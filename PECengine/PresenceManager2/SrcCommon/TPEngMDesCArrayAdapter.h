/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  One value adapter for MDesCArray.
*
*/

#ifndef TPENGMDESCARRAYADAPTER_H__
#define TPENGMDESCARRAYADAPTER_H__

//  INCLUDES
#include <E32Std.h>
#include <BaDesCa.h>


// CLASS DECLARATION
/**
 * One value adapter for MDesCArray.
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( TPEngMDesCArrayAdapter ): public MDesCArray
    {
public: //Constructor & destructor

    /**
     * C++ constructor.
     * Takes the descriptor to adapt.
     */
    TPEngMDesCArrayAdapter( const TDesC& aDesc );


public: //From MDesCArray

    TInt MdcaCount() const;
    TPtrC MdcaPoint( TInt aIndex ) const;


private: //Data

    //OWN: Adapted descriptor
    TPtrC   iDesc;
    };


#endif      //TPENGMDESCARRAYADAPTER_H__

//  End of File




