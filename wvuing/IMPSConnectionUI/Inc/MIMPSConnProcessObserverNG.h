/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  IMPS Connection Process observer.
*
*/

#ifndef __MIMPSCONNPROCESSOBSERVERNG_H
#define __MIMPSCONNPROCESSOBSERVERNG_H

// INCLUDES
#include <E32base.h>
#include <impspresenceconnectionuiconstsng.h>


// FORWARD DECLARATION
class MIMPSConnProcessObserver;
class MIMPSConnProcessUi;
class MIMPSConnProcess;



// CLASS DECLARATION
/**
 * Connection UI observer interface.
 *
 * Connection UI client may implement this interface
 * and give it as connection operation parameter.
 *
 * Observer functions have default implementations
 * implemented in IMPSConnectionUi.dll. Thus the
 * client do not have to implement those observer
 * methods that it doesn't need.
 *
 * @since 2.1
 * @lib IMPSConnectionUi
 */
class MIMPSConnProcessObserver
    {
    public:  // New observation methods

        /**
         * Notifes observer from SAP access events.
         * (Connection UI is logging into or logging
         * out from certain network server.)
         *
         * Note! Access event might come several times
         * during one login/logout operation issued from
         * CIMPSPresenceConnectionUi interface.
         *
         * HandleSapAccessEventEventL() has empty default
         * implementation.
         *
         * @see MIMPSConnProcessUi.
         *
         * @since 2.1
         * @param aServerName The accessed server name.
         * @param aServerURL The accessed server URL.
         * @param aConnProcessUi Connection process UI.
         * If connection process is executed in environment
         * which doesn't have UI, aConnProcessUi is NULL.
         * Else aConnProcessUi is a valid pointer to
         * Connection Process UI interface.
         */
        IMPORT_C virtual void HandleSapAccessEventEventL( const TDesC& aServerName,
                                                          const TDesC& aServerURL,
                                                          MIMPSConnProcessUi* aConnProcessUi );

        /**
         * Reserved virtual table slots for future use.
         * Do not implement these.
         */
        IMPORT_C virtual TInt ReservedObservMethod_1L();
        IMPORT_C virtual TInt ReservedObservMethod_2L();
        IMPORT_C virtual TInt ReservedObservMethod_3L();


    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Concrete observer can't be
         * destroyed using this interface.
         */
        virtual ~MIMPSConnProcessObserver() {};
    };



// CLASS DECLARATION

/**
 * Abstract interface for Connection Process UI.
 *
 * Connection Process UI is provided to
 * MIMPSConnProcessObserver's for handling
 * SAP access event's.
 *
 * @lib IMPSConnectionUi
 * @since 2.1
 **/
class MIMPSConnProcessUi
    {
    public:  // New methods


        /**
         * Sets the new resource file to use in Connection UI.
         * Parameter aNewResourceFile must be full path
         * and filename to the new resource file.
         *
         * Note! Given resourcefile name must point correctly
         * localised resource file for current locale.
         *
         * If empty new resource name is passed, changes the
         * Connection UI to use its default resource file.
         * Client can retrieve this default resource file
         * name using the ResourceFileNameL() method.
         *
         *
         * Handles errors coming from opening the new resource
         * file by leaving.
         *
         * @since 2.1
         * @param aNewResourceFile The resource file to
         * use.
         */
        virtual void UseResourceFileL( const TDesC& aNewResourceFile ) = 0;


        /**
         * Gets Connection UI's resource file name.
         *
         * Gets Connection UI's localised
         * default resource file name.
         *
         * Returns full filename and path. If resource file
         * name doesn't fit in passed aResourceFileName buffer,
         * leaves with KErrOverflow. (Passed buffer's max length
         * should be KMaxFileName.)
         *
         * @since 2.1
         * @param aResourceFileName The resource file name.
         */
        virtual void ResourceFileNameL( TDes& aResourceFileName ) = 0;



    protected:  //Destructor

        /**
         * Virtual inline destructor.
         * Concrete process UI can't be
         * destroyed using this interface.
         */
        virtual ~MIMPSConnProcessUi() {};
    };


#endif      //__MIMPSCONNPROCESSOBSERVERNG_H

//  End of File

