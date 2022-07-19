#include <utils/file-reader.h>

#include <utils/logger.h>
#include <utf8/utf8.h>

namespace mv {
   
  DataFile::DataFile( void ) {

  }

  DataFile::DataFile( const string& filename ) {
    open( filename );
  }

  void DataFile::open( const string& filename ) {
    _wif.open( filename, std::ios::in || std::ios::binary );

    if ( !_wif ) {
      LOG_ERROR( "failed to open file: {}", filename );
      return;
    }

    _wss << _wif.rdbuf();
  }

  string DataFile::content( void ) {
    return _wss.str();
  }

}
