/****************************************************************************
 *  subscription.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "subscription.h"
#include "stanza_p.h"
#include "langmap.h"

namespace jreen
{

J_STRING(presence)
J_STRING(status)

static const QStringList s10n_types = QStringList() << QLatin1String("subscribe") << QLatin1String("subscribed")
									  << QLatin1String("unsubscribe") << QLatin1String("unsubscribed");

struct SubscriptionPrivate : public StanzaPrivate
{
	Subscription::Type subtype;
	LangMap status;
};

Subscription::Subscription( const QDomElement &node ) : Stanza(node, new SubscriptionPrivate)
{
	Q_D(Subscription);
	int type = s10n_types.indexOf( node.attribute( ConstString::type ) );
	d->subtype = type < 0 ? Invalid : static_cast<Type>( type );
	forelements( const QDomElement &elem, node )
		if(  elem.nodeName() == status_str )
		{
			QString lang = elem.attribute( ConstString::lang );
			d->status[lang] = elem.text();
		}
}

Subscription::Subscription( Type type, const JID& to, const QString &status, const QString &xmllang ) : Stanza(new SubscriptionPrivate)
{
	Q_D(Subscription);
	d->subtype = type;
	d->to = to;
	d->status[xmllang] = status;
}

Subscription::Type Subscription::subtype() const
{
	Q_D(const Subscription);
	return d->subtype;
}

const QString &Subscription::status( const QString &lang ) const
{
	Q_D(const Subscription);
	return d->status.value( lang );
}

QDomElement Subscription::node() const
{
	Q_D(const Subscription);
	if( !d->node.isNull() )
		return d->node;
	QDomElement node = DomCreater::instance().createElement( presence_str );
	d->setAttributes( node );
	if( d->subtype == Invalid )
		return node;
	d->status.fillNode( node, status_str );
	node.setAttribute( ConstString::type, s10n_types.at( d->subtype ) );
	d->addExtensions( node );
	return node;
}

}
