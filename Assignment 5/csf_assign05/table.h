#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <string>
#include <pthread.h>
#include <mutex>

class Table {
private:
  std::string m_name;
  // TODO: add member variables

  // Main map for committed key-value pairs.
  std::map<std::string, std::string> committed_data;
    
  // Separate map for tentative changes.
  std::map<std::string, std::string> tentative_data;
    
  // Flag to indicate if there are any tentative changes.
  bool has_tentative_changes = false;

  // Mutex for locking the table during modifications.
  std::mutex table_mutex;

  // copy constructor and assignment operator are prohibited
  Table( const Table & );
  Table &operator=( const Table & );

public:
  Table( const std::string &name );
  ~Table();

  std::string get_name() const { return m_name; }

  void lock();
  void unlock();
  bool trylock();

  // Note: these functions should only be called while the
  // table's lock is held!
  void set( const std::string &key, const std::string &value );
  bool has_key( const std::string &key );
  std::string get( const std::string &key );
  void commit_changes();
  void rollback_changes();
};

#endif // TABLE_H
