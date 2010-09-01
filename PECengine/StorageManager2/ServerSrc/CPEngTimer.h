/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Listen change of the event request handler
*
*/



#ifndef __CPENGTIMER_H__
#define __CPENGTIMER_H__

//  INCLUDES
#include <e32base.h>
#include "MPEngStorageServer.h"

// CLASS DECLARATION
/**
 *  timer for delaying of the Storage Server shut down
 *
 *  @lib PEngStorageServer
 *  @since
 */
class CPEngTimer : public CTimer
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        inline static CPEngTimer* NewL( MPEngStorageServer& aServer )
            {
            CPEngTimer* self = new( ELeave ) CPEngTimer( aServer );
            CleanupStack::PushL( self );
            self->ConstructL();
            CleanupStack::Pop( self );
            return self;
            }

        /**
         * Destructor.
         */
        virtual ~CPEngTimer()
            {
            Cancel();
            }


    public: // Function from CPEngRequestHandler

        /**
         *	Notify client about new event
         *
         *	@since 3.0
         *	@param aNewEvent new event which has occurred
         */
        inline void RunL()
            {
            iServer.TimeExpired();
            }

        /**
         *  Notify Client about error
         *
         *  @since 3.0
         *  @param aErrorCode error code to be notified
         */
        inline TInt RunError( TInt /*aErrorCode*/ )
            {
            return KErrNone;
            }

    private:

        /**
         * C++ default constructor.
         */
        inline CPEngTimer( MPEngStorageServer& aServer )
                : CTimer( CActive::EPriorityStandard ),
                iServer( aServer )
            {
            }


        /**
         *
         */
        inline void ConstructL( )
            {
            CActiveScheduler::Add( this );
            CTimer::ConstructL();
            }

    private:    // Data

        /// Storage server to inform about time out
        MPEngStorageServer& 		iServer;

    };

#endif      // __CPEngTimer_H__

// End of File
