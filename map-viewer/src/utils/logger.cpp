#include <utils/logger.h>

#include <types.h>

namespace mv {

  class custom_formatter_flag : public spdlog::custom_flag_formatter {
  public:
    custom_formatter_flag( size_t line_size = 50 ) : _line_size( line_size ) {}

    virtual void format( const spdlog::details::log_msg& msg, const std::tm& tm, spdlog::memory_buf_t& dest ) override {
      size_t current_size = dest.size();
      string blanks( current_size + 1, ' ' );
      blanks[0] = '\n';

      size_t              current_line_length = 0;
      size_t              prev_index = 0;
      string              message_string( msg.payload.data(), msg.payload.size() );
      std::vector<string> lines;

      for ( size_t i = 0; i < message_string.size(); i++ ) {
        current_line_length++;

        if ( current_line_length > _line_size &&
             ( message_string[i] == ' ' || i == message_string.size() - 1 ) ||
             i == message_string.size() - 1 ) {
          lines.push_back( message_string.substr( prev_index, current_line_length ) );
          current_line_length = 0;
          prev_index = i + 1;
        }
      }

      bool first_line = true;
      for ( const auto& line : lines ) {
        if ( first_line )
          first_line = false;
        else
          dest.append( blanks.data(), blanks.data() + blanks.size() );

        dest.append( line.data(), line.data() + line.size() );
      }
    }

    virtual std::unique_ptr<custom_flag_formatter> clone() const override {
      return spdlog::details::make_unique<custom_formatter_flag>( _line_size );
    }
  private:
    size_t _line_size = 0;
  };

  /* define static variables */
  std::shared_ptr<spdlog::logger> Log::s_logger;

  void Log::init( void ) {
    std::vector<spdlog::sink_ptr> log_sinks;
    log_sinks.emplace_back( std::make_shared<spdlog::sinks::stdout_color_sink_mt>() );

    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<custom_formatter_flag>( '*' ).set_pattern("%^[%=3L] [file: %=18s] [line: %=4#]: %*%$");

    log_sinks[0]->set_formatter( std::move( formatter ) );

    s_logger = std::make_shared<spdlog::logger>( "ENG", log_sinks.begin(), log_sinks.end() );
    spdlog::register_logger( s_logger );
    s_logger->set_level( spdlog::level::trace );
    s_logger->flush_on( spdlog::level::trace );
  }

  std::shared_ptr<spdlog::logger> Log::get_logger( void ) {
    return s_logger;
  }

}
