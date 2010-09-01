/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Inlined function of CCAGroupPropertiesPC.h
*
*/
 
// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: GetTopic
// -----------------------------------------------------------------------------
//
inline TPtrC GetTopic( ) const
	{
	return iGroupPropsForGrpPropDialog->Topic();
    }

// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: GetWelcomeNote
// -----------------------------------------------------------------------------
//
inline TPtrC GetWelcomeNote( ) const
	{
    return iGroupPropsForGrpPropDialog->Welcome();
    }
        
// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: GetMaxNbrOfUsers
// -----------------------------------------------------------------------------
//
inline TInt GetMaxNbrOfUsers( ) const
	{
    return iGroupPropsForGrpPropDialog->MaxNbrOfUsers();
    }
// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: SetGroupNameL
// -----------------------------------------------------------------------------
//
inline void SetGroupNameL(const TDesC& aName ) 
	{
    iGroupPropsForGrpPropDialog->SetGroupNameL( aName );
	}
// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: SetMaxNbrOfUsers
// -----------------------------------------------------------------------------
//
inline void SetMaxNbrOfUsers(TInt aSize ) 
	{
    iGroupPropsForGrpPropDialog->SetMaxNbrOfUsers( aSize );
	}

// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: SetTopicL
// -----------------------------------------------------------------------------
//
inline void SetTopicL(const TDesC& aTopic ) 
	{
    iGroupPropsForGrpPropDialog->SetTopicL( aTopic );
	}
	
// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: SetWelcomeL
// -----------------------------------------------------------------------------
//
inline void SetWelcomeL(const TDesC& aWelcomeNote ) 
	{
    iGroupPropsForGrpPropDialog->SetWelcomeL( aWelcomeNote );
	}
	
// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: SetPrivateAllowed
// -----------------------------------------------------------------------------
//
inline void SetPrivateAllowed(TEnumsPC::TImpsPropertyBoolPC aValue ) 
	{
    iGroupPropsForGrpPropDialog->SetPrivateAllowed( static_cast<TImpsPropertyBool>(aValue ));
	}	
// -----------------------------------------------------------------------------
// CCAGroupPropertiesPC: SetSearchable
// -----------------------------------------------------------------------------
//
inline void SetSearchable( TEnumsPC::TImpsPropertyBoolPC aType ) 
	{
    iGroupPropsForGrpPropDialog->SetSearchable( static_cast<TImpsPropertyBool>(aType) );
	}	
