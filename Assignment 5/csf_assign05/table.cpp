#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table( const std::string &name )
  : m_name( name )
{
}

Table::~Table()
{
}

void Table::lock()
{
  table_mutex.lock();
}

void Table::unlock()
{
  table_mutex.unlock();
}

bool Table::trylock()
{
  return table_mutex.try_lock();
}

void Table::set( const std::string &key, const std::string &value )
{
  if (committed_data.find(key) != committed_data.end()) {
    if (committed_data.at(key) != value) {
      has_tentative_changes = true;
    }
  }
  tentative_data[key] = value;

}

std::string Table::get( const std::string &key )
{
  for (const auto& tuple: tentative_data) {
    if (tuple.first == key) {
      return tuple.second;
    }
  }

  for (const auto& tuple: committed_data) {
    if (tuple.first == key) {
      return tuple.second;
    }
  }

  throw std::out_of_range("Element could not be found");
}

bool Table::has_key( const std::string &key )
{
  bool tentative_has_key = tentative_data.find(key) != tentative_data.end();
  bool committed_has_key = committed_data.find(key) != committed_data.end();
  return tentative_has_key || committed_has_key;
}

void Table::commit_changes()
{
  committed_data = tentative_data;
  has_tentative_changes = false;
}

void Table::rollback_changes()
{
  tentative_data = committed_data;
  has_tentative_changes = false;
}
