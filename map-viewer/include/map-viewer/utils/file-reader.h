#pragma once

#include <fstream>
#include <sstream>
#include <codecvt>
#include <ios>

#include <types.h>

namespace mv {

  class DataFile {
  public:
    DataFile( void );
    DataFile( const string& filename );
    ~DataFile( void ) = default;
  public:
    void open( const string& filename );
    string content( void );

    operator bool( void ) const { return _wif.operator bool(); }
  private:
    std::stringstream _wss;
    std::ifstream     _wif;
  };

}
