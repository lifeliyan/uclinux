/*
 * Presence Agent, subscribe handling
 *
 * $Id: subscribe.h,v 1.4 2003/05/02 08:37:53 janakj Exp $
 *
 * Copyright (C) 2001-2003 Fhg Fokus
 *
 * This file is part of ser, a free SIP server.
 *
 * ser is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * For a license to use the ser software under conditions
 * other than those described here, or to purchase support for this
 * software, please contact iptel.org by e-mail at the following addresses:
 *    info@iptel.org
 *
 * ser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H

#include "../../parser/msg_parser.h"


/*
 * Handle a subscribe Request
 */
int handle_subscription(struct sip_msg* _m, char* _domain, char* _s2);


/*
 * Return 1 if the subscription exists and 0 if not
 */
int existing_subscription(struct sip_msg* _m, char* _domain, char* _s2);


/*
 * Returns 1 if possibly a user agent can handle SUBSCRIBE
 * itself, 0 if it cannot for sure
 */
int pua_exists(struct sip_msg* _m, char* _domain, char* _s2);


#endif /* SUBSCRIBE_H */
