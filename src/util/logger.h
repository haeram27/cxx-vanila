#pragma once

#include <iostream>
#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h" // support for stdout logging
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging
#include "spdlog/sinks/daily_file_sink.h" // support for daily file logging
#include "spdlog/sinks/rotating_file_sink.h" // support for rotating file logging

namespace util{
namespace logger{

void init() {
    try {
        /*** console_logger ***/
        spdlog::logger logger("console_logger", std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));

        /*** basic_file_logger ***/
        // spdlog::logger logger("file_logger", std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true));
        // spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));

        /*** multi_sink_logger: stdout + file ***/
        // auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
        // spdlog::sinks_init_list sink_list = { file_sink, console_sink };
        // spdlog::logger logger("multi_sink_logger", sink_list.begin(), sink_list.end());
        // spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));

        /*** rotating_file_logger: 5MB size, 3 maxfile ***/
        // auto logger = spdlog::rotating_logger_mt("rotating_file_logger", "logs/rotating_log.txt", 1048576 * 5, 3);
        // spdlog::set_default_logger(logger);

        /*** async_rotating_file_logger: 5MB size, 3 maxfile ***/
        // auto logger = spdlog::rotating_logger_mt<spdlog::async_factory>("async_rotating_file_logger", "logs/async_rotating_log.txt", 1048576 * 5, 3);
        // spdlog::set_default_logger(logger);

        /*** daily_file_logger: 5 maxfile ***/
        // auto logger = spdlog::daily_logger_mt("daily_file_logger", "logs/daily_log.txt", 0, 0, false, 3);
        // spdlog::set_default_logger(logger);

        /*** async_daily_file_logger: 5 maxfile ***/
        // auto logger = spdlog::daily_logger_mt<spdlog::async_factory>("async_daily_file_logger", "logs/async_daily_log.txt", 0, 0, false, 3);
        // spdlog::set_default_logger(logger);

        /*** async_daily_file_logger: 5 maxfile ***/
        // auto logger = spdlog::create_async<spdlog::sinks::daily_file_sink_mt>("async_daily_file_logger", "logs/async_daily_log.txt", 0, 0, false, 3);
        // spdlog::set_default_logger(logger);

        /*** multi_sink_logger: stdout + daily file ***/
        // auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // auto daily_file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/daily_log.txt", 0, 0, false, 3);
        // spdlog::sinks_init_list sink_list = { daily_file_sink, console_sink };
        // spdlog::logger logger("multi_sink_logger", sink_list.begin(), sink_list.end());
        // spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));


        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][%P-%t][%^%l%$] %v ");
        // spdlog::set_level(spdlog::level::trace);
        // spdlog::flush_every(static_cast<std::chrono::seconds>(1));
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Logger initialization failed: " << ex.what() << std::endl;
    }


// #define FORKTEST
// #define LOGTEST
#if defined(LOGTEST)
#if defined(FORKTEST)
    for (int f=1; f<10; ++f) {
        if(fork()==0) {
#endif //FORKTEST
        for (int i=1; i<1000; ++i) {
            spdlog::trace("spdlog init OK #{}", i);
            SPDLOG_TRACE("spdlog init OK #{}", i);
            // SPDLOG_LOGGER_TRACE(logger_name, "spdlog init OK #{}", i);
            spdlog::debug("spdlog init OK #{}", i);
            SPDLOG_DEBUG("spdlog init OK #{}", i);
            // SPDLOG_LOGGER_DEBUG(logger_name, "spdlog init OK #{}", i);
            spdlog::info("spdlog init OK #{}", i);
            SPDLOG_INFO("spdlog init OK #{}", i);
            // SPDLOG_LOGGER_INFO(logger_name, "spdlog init OK #{}", i);
            spdlog::warn("spdlog init OK #{}", i);
            SPDLOG_WARN("spdlog init OK #{}", i);
            // SPDLOG_LOGGER_WARN(logger_name, "spdlog init OK #{}", i);
            spdlog::error("spdlog init OK #{}", i);
            SPDLOG_ERROR("spdlog init OK #{}", i);
            // SPDLOG_LOGGER_ERROR(logger_name, "spdlog init OK #{}", i);
            spdlog::critical("spdlog init OK #{}", i);
            SPDLOG_CRITICAL("spdlog init OK #{}", i);
            // SPDLOG_LOGGER_CRITICAL(logger_name, "spdlog init OK #{}", i);
        }
#if defined(FORKTEST)
        }
    }
#endif //FORKTEST
#endif //LOGTEST
}

} // namespace logger
} // namespace util