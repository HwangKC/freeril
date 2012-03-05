/*
 * USB.h
 *
 *  Created on: 01.03.2012
 *      Author: Swen Kuehnlein
 */

#ifndef USB_H_
#define USB_H_

namespace freeril
{
	class USB {
	public:
		static USB& instance();
		virtual ~USB();
	protected:
		USB();
	};
}

#endif /* USB_H_ */
