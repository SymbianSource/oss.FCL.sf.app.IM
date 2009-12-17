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
* Description:  CPEngJanitorPlugin.h
*
*/

#ifndef __CPENGJANITORPLUGIN_H__
#define __CPENGJANITORPLUGIN_H__

//  INCLUDES
#include <CPEngPluginInterface2.h>
#include <E32Base.h>


// FORWARD DECLARATION
class CPEngContactMapper;

//  CLASS DEFINITION
/**
 * CPEngJanitorPlugin
 *
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngJanitorPlugin )
        : public CPEngPluginInterface2
    {
public:     // Constructors and destructors

    /**
     * Instantiates CPEngJanitorPlugin object.
     *
     * @return New CPEngJanitorPlugin instance.
     */
    static CPEngJanitorPlugin* NewL();


    /**
     * Destructor.
     * Virtual by CBase.
     */
    ~CPEngJanitorPlugin();




private:  // Constructors and destructors

    /**
     * C++ default constructor.
     */
    CPEngJanitorPlugin();


    /**
     * Symbian OS constructor.
     */
    void ConstructL();

private:  // Data

    CPEngContactMapper* 	iContactMapper;

    };

#endif      //  __CPENGJANITORPLUGIN_H__


// End of file

