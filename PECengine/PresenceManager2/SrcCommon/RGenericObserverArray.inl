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
* Description:  Inline methods for thin templated RGenericObserverArray
*
*/



// ================= HELPER CLASS =======================
/**
 * Helper inline class to forward requests
 * from base mediator (typeless) to type specific
 * mediator.
 *
 * @since 3.0
 */
template< class T >
class TGenObserverNotifyMediatorBase : public MGenObserverNotifyMediatorBase
    {
    public:
        inline TGenObserverNotifyMediatorBase( MGenObserverNotifyMediator< T >& aSpecificMediator )
            : iSpecificMediator( aSpecificMediator ){}

    private:
        inline void MediateNotifyL( TAny* aObserverToNotify )
            { iSpecificMediator.MediateNotifyL( *( static_cast<T*>( aObserverToNotify ) ) ); }

        inline void MediateNotifyError( TAny* aObserverToNotify, TInt aLeaveError )
            { iSpecificMediator.MediateNotifyError( *( static_cast<T*>( aObserverToNotify ) ), aLeaveError ); }

    private:
        ///< Observer type specific notify mediator, not owned
        MGenObserverNotifyMediator< T >& iSpecificMediator;
        };


// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
template < class T >
inline RGenericObserverArray<T >::RGenericObserverArray()
    {
    }

// -----------------------------------------------------------------------------
// RGenericObserverArray::Close()
// -----------------------------------------------------------------------------
//
template < class T >
inline void RGenericObserverArray< T >::Close()
    {
    RGenericObserverArrayBase::Close();
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::AddObserver()
// -----------------------------------------------------------------------------
//
template < class T >
inline TInt RGenericObserverArray< T >::AddObserver( const T* aObserver )
    {
    return RGenericObserverArrayBase::AddObserver( static_cast<const TAny*>( aObserver ) );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::AddObserverL()
// -----------------------------------------------------------------------------
//
template < class T >
inline void RGenericObserverArray< T >::AddObserverL( const T* aObserver )
    {
    User::LeaveIfError( AddObserver( aObserver ) );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::RemoveObserver()
// -----------------------------------------------------------------------------
//
template < class T >
inline TInt RGenericObserverArray< T >::RemoveObserver( T* aObserver )
    {
    return RGenericObserverArrayBase::RemoveObserver( static_cast<TAny*>( aObserver ) );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::Count()
// -----------------------------------------------------------------------------
//
template < class T >
inline TInt RGenericObserverArray< T >::Count()
    {
    return RGenericObserverArrayBase::Count();
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::NotifyObservers()
// -----------------------------------------------------------------------------
//
template < class T >
inline void RGenericObserverArray< T >::NotifyObservers( MGenObserverNotifyMediator< T >& aNotifyMediator )
    {
    TGenObserverNotifyMediatorBase< T > baseMediator( aNotifyMediator );
    RGenericObserverArrayBase::NotifyObservers( baseMediator );
    }




// -----------------------------------------------------------------------------
// RGenericObserverArray::NotifyErrorObservers()
// -----------------------------------------------------------------------------
//
template < class T >
inline void RGenericObserverArray< T >::NotifyErrorObservers( MGenObserverNotifyMediator< T >& aNotifyMediator,
                                                              TInt aError )
    {
    TGenObserverNotifyMediatorBase< T > baseMediator( aNotifyMediator );
    RGenericObserverArrayBase::NotifyErrorObservers( baseMediator, aError );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::NotifyLoopRunning()
// -----------------------------------------------------------------------------
//
template < class T >
inline TBool RGenericObserverArray< T >::NotifyLoopRunning() const
    {
    return RGenericObserverArrayBase::IsNotifyLoopRunning();
    }


//  End of File

