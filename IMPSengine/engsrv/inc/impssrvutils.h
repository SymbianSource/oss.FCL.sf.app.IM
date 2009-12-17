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
* Description: Util classes for IMPS service
* 
*/


#ifndef CIMPSSRVUTILS_H
#define CIMPSSRVUTILS_H

//  INCLUDES
#include <e32base.h>
#include <s32file.h>

#include "impsservercommon.h"
#include "impsservices.h"


// FORWARD DECLARATIONS
class CImpsDataAccessor;
class CImpsServer;
class MImpsKey;
class MImpsDataAccessor;


// CLASS DECLARATION

class TImpsSrvSettings
    {
    public:  // Constructors and destructor

        TImpsSrvSettings( );

    public:  // New methods

        /**
        * Read static settings
        */
        void ReadStaticSettings();

#ifdef _DEBUG
        /**
        * Read ini file, DEBUG mode only
        */
        void ReadIniFileL( RFs&  aFs );
#endif

        /**
        * Set Cir channel info
        * @param aAccess data accessor pointing to parent of CIR method list
        * @param aKey data key instance
        */
        void SetCirBindingsL( MImpsDataAccessor* aAcess, MImpsKey* aKey );

        /**
        * Accessors for CIR channel data
        */
        inline TInt UDPWAPCIR() const;
        inline TInt SMSWAPCIR() const;
        inline TInt TCPSCIR() const;
        inline TInt UDPSCIR() const;

        /**
        * Accessor for PDP expiry time
        * @return expiry time in seconds
        */
        inline TInt PDPExpiryTime() const;

        /**
        * Accessor for CIR mode poll time
        * @return poll time in seconds
        */
        inline TInt CIRModePollTime() const;

        /**
        * Accessor for Maximum Message Size
        * @return Accepted content Length
        */
        inline TInt MaximumMessageSize() const;

        /**
        * Calculates Maximum Parser size based on maximum message size
        */
        inline TInt MaximumParserSize() const;

    private:    // Data
        TInt           iPollTime;
        TInt           iAliveTime;
        TInt           iMaxPollTime;
        TInt           iXmlLog;        // Is CSP primitives logged
        // 1 Messages are logged
        // 2 Polling is also logged

        TInt           iUDPWAPCIR;     // Is CIR available, 1=ordered,2=accepted
        TInt           iSMSWAPCIR;     // Is CIR available
        TInt           iTCPSCIR;       // Is CIR available
        TInt           iUDPSCIR;       // Is CIR available

        TInt           iAccessExp;     // Access expiry time in seconds
        TInt           iImExp;         // IM expiry time
        TInt           iPrExp;         // Presence expiry time
        TInt           iGrExp;         // Group expiry time
        TInt           iFuExp;         // Fundamental expiry time

        TInt           iMultiTrans;    // CSP MultiTrans
        TBool          iFourWayLogin;  // Use 4-Way login

        TInt           iPDPExpiry;     // PDP expiry in seconds
        TInt           iCIRModePollTime; // Poll time in CIR mode

        TInt           iMaxMessageSize; // Max IM message size to be negotiated

    private:
        friend class CImpsServer;
        friend class CImpsCSPSession;

    };


class TImpsSrvUtils
    {
    public:  // New methods

        /**
        * Initialize suggested CSP services
        * @param aServices service tree
        * @param aReactive reactive presence auth negotiation
        */
        static void InitializeServices(
            TImpsServices& aServices,
            TBool aReactive );

        /**
        * Discard CSP services based on response from SAP
        * @param aData response service tree
        * @param aServices service tree OUTPUT
        */
        static void DiscardServicesL(
            MImpsDataAccessor* aData,
            TImpsServices* aServices );

        /**
        * Check availability of the service
        * @param aFeature required feature
        * @param aSrvS server features
        * Leave with KErrNotSupported if not available
        */
        static void CheckServiceL(
            TImpsFeatures aFeature,
            TImpsServices* aSrvS );

    private:  // methods

    };

// Identification of unique CSP session.
// Notice that this class does not store any data and thus this
// can be used only for passing CSP identification in function calls.
// References to data members are temporary only and may not be valid
// after function call, and thus each class entity should make an own
// copy of data if it needs that data later on.
class TImpsSessIdent
    {
    public:
        /**
        * Constructor
        * @param aSID CSP session id
        * @param aSAP SAP address
        * @param aUserId WV user id
        */
        TImpsSessIdent( const TDesC& aSID,
                        const TDesC& aSAP, const TDesC& aUserId );

    public:  // New methods

        /**
        * Accessors
        */
        inline TPtrC SID() const;
        inline TPtrC SAP() const;
        inline TPtrC UserId() const;

        /**
        * check if all fields are zero length
        * @return ETrue if all is zero
        */
        inline TBool IsZero() const;

    private:    // Data
        const TDesC&           iSID;
        const TDesC&           iSAP;
        const TDesC&           iUserId;

    };


inline TInt TImpsSrvSettings::UDPWAPCIR() const
    {
    return iUDPWAPCIR;
    }

inline TInt TImpsSrvSettings::SMSWAPCIR() const
    {
    return iSMSWAPCIR;
    }

inline TInt TImpsSrvSettings::TCPSCIR() const
    {
    return iTCPSCIR;
    }

inline TInt TImpsSrvSettings::UDPSCIR() const
    {
    return iUDPSCIR;
    }

inline TInt TImpsSrvSettings::PDPExpiryTime() const
    {
    return iPDPExpiry;
    }

inline TInt TImpsSrvSettings::CIRModePollTime() const
    {
    return iCIRModePollTime;
    }

inline TInt TImpsSrvSettings::MaximumMessageSize() const
    {
    return iMaxMessageSize;
    }

inline TInt TImpsSrvSettings::MaximumParserSize() const
    {
    // Max( msg size + 2K ; 1 MB )
    // Remember that presence, group messages etc are not limited
    // by msg size, it's only for IM.
    return Max( ( iMaxMessageSize + 2048 ), KImpsParserSizeMax );
    }

inline TPtrC TImpsSessIdent::SID() const
    {
    return iSID;
    }

inline TPtrC TImpsSessIdent::SAP() const
    {
    return iSAP;
    }

inline TPtrC TImpsSessIdent::UserId() const
    {
    return iUserId;
    }

inline TBool TImpsSessIdent::IsZero() const
    {
    return ( !iSAP.Length() && !iSID.Length() && !iUserId.Length() ) ? ETrue : EFalse;
    }

#endif      // ?INCLUDE_H   

// End of File
