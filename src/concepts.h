/*
 * concepts.h
 *
 *  Created on: 09.03.2012
 *      Author: Swen Kühnlein
 */

#ifndef FREERIL_CONCEPTS_H_
#define FREERIL_CONCEPTS_H_

namespace freeril
{

class NotCopyable
{
public:
	NotCopyable() {}
private:
	// No implementation provided
	NotCopyable(const NotCopyable&);
};

class NotAssignable
{
public:
	NotAssignable() {}
private:
	// No implementation provided
	void operator=(const NotAssignable&);
};

} // namespace freeril


#endif /* FREERIL_CONCEPTS_H_ */
