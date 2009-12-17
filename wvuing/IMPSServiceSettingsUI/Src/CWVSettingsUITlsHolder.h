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
* Description:  Tls holder used to check if resource file is loaded or not
*
*/

#ifndef CWVSETTINGSUITLSHOLDER_H
#define CWVSETTINGSUITLSHOLDER_H

//  INCLUDES
#include <E32Base.h>


// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  TLS holder used to check if resource file is loaded or not
*  and if it can be unloaded or not.
*
*  @lib WVServiceSettingsUi.dll
*  @since 2.1
*/
NONSHARABLE_CLASS( CWVSettingsUITlsHolder ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Singleton pointer retrieve method.
     */
    static CWVSettingsUITlsHolder* InstanceL();


    /**
     * Trys to delete all owned singletons and
     * also this holder object.
     * If NULLing the TLS pointer, pointing to this
     * holder object fails, doesn't delete holder, only
     * owned singletons. After that this TLS holder can
     * be still reused, by issuing again first InstanceL()
     * and to the retrived pointer one of existing sinleton
     * retrieve requests,
     *
     * NOTE!! After calling DestroySingletonsAndHolder()
     * all singleton object pointers must be retrived
     * again before using them!!
     *
     * @since 2.1
     * @return If fully succesfull  KErrNone. Else one of
     * system wide errorcodes.
     */
    static TInt DestroySingletonsAndHolder();


public: // New functions

    /**
    * Tells if the resource is loaded
    * @since 2.1
    */
    TBool ResourceIsLoaded() const;

    /**
    * Adds one to the reference count of the resource file
    * @since 2.1
    */
    void ResourceLoaded();

    /**
    * Removes one to the reference count of the resource file
    * @since 2.1
    */
    void ResourceUnLoaded();

    TBool OkToUnloadResource() const;

private: //new functions


    /**
     * Deletes owned singleton objects.
     *
     * @since 2.1
     */
    void DoReleaseSingletons();

private:

    /**
    * Two-phased constructor.
    * Leaves created object on the cleanupstack.
    * @since 2.1
    */
    static CWVSettingsUITlsHolder* NewLC();


    /**
    * C++ default constructor.
    */
    CWVSettingsUITlsHolder();


    /**
    * Protected destructor.
    *
    * Singleton holder can be only destroyed trough
    * DestroySingletonsAndHolder() method.
    */
    virtual ~CWVSettingsUITlsHolder();


private:    // Data

    ///Publish level manager singleton, owned
    TBool iResourceLoaded;

    ///Spare member for future extensions
    TAny* iReserved;

    TInt iResourceReferenceCount;

    };

#endif      // CWVSETTINGSUITLSHOLDER_H

// End of File
