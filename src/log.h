/*
 * log.h
 *
 *  Created on: 03.03.2012
 *      Author: Swen Kühnlein
 */

#ifndef FREERIL_LOG_H_
#define FREERIL_LOG_H_

#include <string>

#define LOGV(from, msg) logmsg(from, log::VERBOSE, msg)
#define LOGD(from, msg) logmsg(from, log::DEBUG, msg)
#define LOGI(from, msg) logmsg(from, log::INFO, msg)
#define LOGW(from, msg) logmsg(from, log::WARN, msg)
#define LOGE(from, msg) logmsg(from, log::ERROR, msg)

#define CLOGV(msg) LOGV(this, msg)
#define CLOGD(msg) LOGD(this, msg)
#define CLOGI(msg) LOGI(this, msg)
#define CLOGW(msg) LOGW(this, msg)
#define CLOGE(msg) LOGE(this, msg)

namespace freeril
{

class Loggable
{
public:
	virtual ~Loggable();
	virtual std::string logName() const;
};

namespace log
{
enum Priority
{
	DEFAULT = 0,
	VERBOSE,
	DEBUG,
	INFO,
	WARN,
	ERROR
};
} // namespace log

void logmsg(
		const Loggable* from,
		const log::Priority priority,
		const std::string& msg);
void logmsg(
		const std::string& from,
		const log::Priority priority,
		const std::string& msg);

} // namespace freeril

#endif /* FREERIL_LOG_H_ */
