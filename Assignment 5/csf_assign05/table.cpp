#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table(const std::string &name) : m_name(name) {
  // Try to initialize mutex and throw an error if unsuccessful
  if (pthread_mutex_init(&table_mutex, nullptr) != 0) {
    throw std::runtime_error("Failed to initialize mutex");
  }
}

Table::~Table() {
  // Destroy the thread once the destructor for the Table is created
  pthread_mutex_destroy(&table_mutex);   
}

void Table::lock() {
  // Lock the thread using the table mutex
  pthread_mutex_lock(&table_mutex);
}

void Table::unlock() {
  // Unlock the thread using the table mutex
  pthread_mutex_unlock(&table_mutex);
}

bool Table::trylock() {
  // Try to unlock the thread using trylock and return its success
  return pthread_mutex_trylock(&table_mutex) == 0;
}

void Table::set( const std::string &key, const std::string &value ) {
  // Set the tentative changes equal to true if the value is different from the commmitted values
  if (committed_data.find(key) != committed_data.end()) {
    if (committed_data.at(key) != value) {
      has_tentative_changes = true;
    }
  }

  // Set the tentative_data at the key equal to the value
  tentative_data[key] = value;
}

std::string Table::get( const std::string &key ) {
  // Look through the tentative map to see if you have the key
  for (const auto& tuple: tentative_data) {
    // Return the data if the key matches
    if (tuple.first == key) {
      return tuple.second;
    }
  }

  // If not found in the tentative data, look through the committed data
  for (const auto& tuple: committed_data) {
    // Return the data if the key matches
    if (tuple.first == key) {
      return tuple.second;
    }
  }

  // Key does not exist; throw an error
  throw std::out_of_range("Element could not be found");
}

bool Table::has_key( const std::string &key ) {
  bool tentative_has_key = tentative_data.find(key) != tentative_data.end();
  bool committed_has_key = committed_data.find(key) != committed_data.end();
  return tentative_has_key || committed_has_key;
}

void Table::commit_changes() {
  // Set the committed map equal to the tentative map to save the changes
  committed_data = tentative_data;

  // There are no more tentative changes
  has_tentative_changes = false;
}

void Table::rollback_changes() {
  // Revert the tentative map back to the committed map
  tentative_data = committed_data;

  // There are no more tentative changes
  has_tentative_changes = false;
}
