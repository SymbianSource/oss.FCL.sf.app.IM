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
 * @since 2.1
 */
template <class T, class P>
class TGenObserverNotifyMediatorBase : public MGenObserverNotifyMediatorBase
    {
    public:
        inline TGenObserverNotifyMediatorBase( MGenObserverNotifyMediator<T, P>& aSpecificMediator )
            : iSpecificMediator( aSpecificMediator ){}

    private:
        inline void MediateNotifyL( TAny* aObserverToNotify, TAny* aNotifyData )
            { iSpecificMediator.MediateNotifyL( *( static_cast<T*>( aObserverToNotify ) ),
                                                *( static_cast<P*>( aNotifyData ) ) ); }

        inline void MediateNotifyError( TAny* aObserverToNotify, TInt aLeaveError )
            { iSpecificMediator.MediateNotifyError( *( static_cast<T*>( aObserverToNotify ) ), aLeaveError ); }

        inline void MediateError( TAny* aObserverToNotify, TInt aError )
            { iSpecificMediator.MediateError( *( static_cast<T*>( aObserverToNotify ) ), aError ); }

    private:
        ///< Observer type specific notify mediator, not owned
        MGenObserverNotifyMediator<T, P>& iSpecificMediator;
        };


// ================= MEMBER FUNCTIONS =======================
// C++ default constructor can NOT contain any code, that
// might leave.
//
template <class T, class P>
inline RGenericObserverArray<T, P>::RGenericObserverArray()
    {
    }

// -----------------------------------------------------------------------------
// RGenericObserverArray::Close()
// -----------------------------------------------------------------------------
//
template <class T, class P>
inline void RGenericObserverArray<T, P>::Close()
    {
    RGenericObserverArrayBase::Close();
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::AddObserver()
// -----------------------------------------------------------------------------
//
template <class T, class P>
inline TInt RGenericObserverArray<T, P>::AddObserver( const T* aObserver )
    {
    return RGenericObserverArrayBase::AddObserver( static_cast<const TAny*>( aObserver ) );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::AddObserverL()
// -----------------------------------------------------------------------------
//
template <class T, class P>
inline void RGenericObserverArray<T, P>::AddObserverL( const T* aObserver )
    {
    User::LeaveIfError( AddObserver( aObserver ) );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::RemoveObserver()
// -----------------------------------------------------------------------------
//
template <class T, class P>
inline TInt RGenericObserverArray<T, P>::RemoveObserver( T* aObserver )
    {
    return RGenericObserverArrayBase::RemoveObserver( static_cast<TAny*>( aObserver ) );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::Count()
// -----------------------------------------------------------------------------
//
template <class T, class P>
inline TInt RGenericObserverArray<T, P>::Count()
    {
    return RGenericObserverArrayBase::Count();
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::NotifyObservers()
// -----------------------------------------------------------------------------
//
template <class T, class P>
inline void RGenericObserverArray<T, P>::NotifyObservers( MGenObserverNotifyMediator<T, P>& aNotifyMediator,
                                                          P& aNotifyData )
    {
    TGenObserverNotifyMediatorBase<T, P> baseMediator( aNotifyMediator );
    RGenericObserverArrayBase::NotifyObservers( baseMediator,
                                                static_cast<TAny*>( &aNotifyData) );
    }


// -----------------------------------------------------------------------------
// RGenericObserverArray::NotifyObserversFromError()
// -----------------------------------------------------------------------------
//
template <class T, class P>
inline void RGenericObserverArray<T, P>::NotifyObserversFromError( MGenObserverNotifyMediator<T, P>& aNotifyMediator,
                                                                   TInt aError )
    {
    TGenObserverNotifyMediatorBase<T, P> baseMediator( aNotifyMediator );
    RGenericObserverArrayBase::NotifyObserversFromError( baseMediator, aError );
    }

//  End of File

