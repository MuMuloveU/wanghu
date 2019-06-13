/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Jörg König
// All rights reserved
//
// This file is part of the completely free tetris clone "CGTetris".
//
// This is free software.
// You may redistribute it by any means providing it is not sold for profit
// without the authors written consent.
//
// No warrantee of any kind, expressed or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    J.Koenig@adg.de                 (company site)
//    Joerg.Koenig@rhein-neckar.de    (private site)
/////////////////////////////////////////////////////////////////////////////


// DirectSound.cpp: implementation of the CDirectSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectSound.h"

// The following macro is defined since DirectX 5, but will work with
// older versions too.
#ifndef DSBLOCK_ENTIREBUFFER
	#define DSBLOCK_ENTIREBUFFER        0x00000002
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static void DSError( HRESULT hRes ) {
	switch(hRes) {
		case DS_OK: TRACE0("NO ERROR\n"); break;
		case DSERR_ALLOCATED: TRACE0("ALLOCATED\n"); break;
		case DSERR_INVALIDPARAM: TRACE0("INVALIDPARAM\n"); break;
		case DSERR_OUTOFMEMORY: TRACE0("OUTOFMEMORY\n"); break;
		case DSERR_UNSUPPORTED: TRACE0("UNSUPPORTED\n"); break;
		case DSERR_NOAGGREGATION: TRACE0("NOAGGREGATION\n"); break;
		case DSERR_UNINITIALIZED: TRACE0("UNINITIALIZED\n"); break;
		case DSERR_BADFORMAT: TRACE0("BADFORMAT\n"); break;
		case DSERR_ALREADYINITIALIZED: TRACE0("ALREADYINITIALIZED\n"); break;
		case DSERR_BUFFERLOST: TRACE0("BUFFERLOST\n"); break;
		case DSERR_CONTROLUNAVAIL: TRACE0("CONTROLUNAVAIL\n"); break;
		case DSERR_GENERIC: TRACE0("GENERIC\n"); break;
		case DSERR_INVALIDCALL: TRACE0("INVALIDCALL\n"); break;
		case DSERR_OTHERAPPHASPRIO: TRACE0("OTHERAPPHASPRIO\n"); break;
		case DSERR_PRIOLEVELNEEDED: TRACE0("PRIOLEVELNEEDED\n"); break;
		default: TRACE1("%lu\n",hRes);break;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LPDIRECTSOUND CDirectSound::m_lpDirectSound;
DWORD CDirectSound::m_dwInstances;


CDirectSound::CDirectSound()
{
	m_lpDirectSound = 0;
	m_pDsb = 0;
	m_pTheSound = 0;
	m_dwTheSound = 0;
	m_bEnabled = TRUE;

	++m_dwInstances;
}

CDirectSound::~CDirectSound()
{
	if( m_pDsb )
		m_pDsb->Release();

	if( !--m_dwInstances && m_lpDirectSound ) {
		m_lpDirectSound->Release();
		m_lpDirectSound = 0;
	}
}

BOOL CDirectSound::Create(LPCTSTR pszResource, CWnd * pWnd)
{
	//////////////////////////////////////////////////////////////////
	// load resource
	HINSTANCE hApp = ::GetModuleHandle(0);
	ASSERT(hApp);

	HRSRC hResInfo = ::FindResource(hApp, pszResource, TEXT("WAVE"));
	if(hResInfo == 0)
		return FALSE;

	HGLOBAL hRes = ::LoadResource(hApp, hResInfo);
	if(hRes == 0)
		return FALSE;

	LPVOID pTheSound = ::LockResource(hRes);
	if(pTheSound == 0)
		return FALSE;

	return Create(pTheSound, pWnd);
}


BOOL CDirectSound :: Create(LPVOID pSoundData, CWnd * pWnd) {
	if(pWnd == 0)
		pWnd = AfxGetApp()->GetMainWnd();

	ASSERT(pWnd != 0);
	ASSERT(::IsWindow(pWnd->GetSafeHwnd()));

	ASSERT(pSoundData != 0);

	//////////////////////////////////////////////////////////////////
	// create direct sound object
	
	if( m_lpDirectSound == 0 ) {
		// Someone might use sounds for starting apps. This may cause
		// DirectSoundCreate() to fail because the driver is used by
		// anyone else. So wait a little before starting with the work ...
		HRESULT hRes = DS_OK;
		short nRes = 0;

		do {
			if( nRes )
				::Sleep(500);
			hRes = ::DirectSoundCreate(0, &m_lpDirectSound, 0);
			++nRes;
		} while( nRes < 10 && (hRes == DSERR_ALLOCATED || hRes == DSERR_NODRIVER) );

		if( hRes != DS_OK )
			return FALSE;
	
		m_lpDirectSound->SetCooperativeLevel(pWnd->GetSafeHwnd(), DSSCL_NORMAL);
	}

	ASSERT(m_lpDirectSound != 0);

	WAVEFORMATEX * pcmwf;
	if( ! GetWaveData(pSoundData, pcmwf, m_pTheSound, m_dwTheSound) ||
		! CreateSoundBuffer(pcmwf) ||
		! SetSoundData(m_pTheSound, m_dwTheSound) )
		return FALSE;

	return TRUE;
}


BOOL CDirectSound :: GetWaveData(void * pRes, WAVEFORMATEX * & pWaveHeader, void * & pbWaveData, DWORD & cbWaveSize) {
	pWaveHeader = 0;
	pbWaveData = 0;
	cbWaveSize = 0;

	DWORD * pdw = (DWORD *)pRes;
	DWORD dwRiff = *pdw++;
	DWORD dwLength = *pdw++;
	DWORD dwType = *pdw++;

	if( dwRiff != mmioFOURCC('R', 'I', 'F', 'F') )
		return FALSE;      // not even RIFF

	if( dwType != mmioFOURCC('W', 'A', 'V', 'E') )
		return FALSE;      // not a WAV

	DWORD * pdwEnd = (DWORD *)((BYTE *)pdw + dwLength-4);

	while( pdw < pdwEnd ) {
		dwType = *pdw++;
		dwLength = *pdw++;

		switch( dwType ) {
			case mmioFOURCC('f', 'm', 't', ' '):
				if( !pWaveHeader ) {
					if( dwLength < sizeof(WAVEFORMAT) )
						return FALSE;      // not a WAV

					pWaveHeader = (WAVEFORMATEX *)pdw;

					if( pbWaveData && cbWaveSize )
						return TRUE;
				}
				break;

			case mmioFOURCC('d', 'a', 't', 'a'):
				pbWaveData = LPVOID(pdw);
				cbWaveSize = dwLength;

				if( pWaveHeader )
					return TRUE;
				break;
		}
		pdw = (DWORD *)((BYTE *)pdw + ((dwLength+1)&~1));
	}

	return FALSE;
}


BOOL CDirectSound::CreateSoundBuffer(WAVEFORMATEX * pcmwf)
{
	DSBUFFERDESC dsbdesc;

	// Set up DSBUFFERDESC structure.
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	// Need no controls (pan, volume, frequency).
	dsbdesc.dwFlags = DSBCAPS_STATIC;		// assumes that the sound is played often
	dsbdesc.dwBufferBytes = m_dwTheSound;
	dsbdesc.lpwfxFormat = pcmwf;    // Create buffer.
	HRESULT hRes;
	if( DS_OK != (hRes = m_lpDirectSound->CreateSoundBuffer(&dsbdesc, &m_pDsb, 0)) ) {
		// Failed.
		DSError(hRes);
		m_pDsb = 0;
		return FALSE;
	}

	return TRUE;
}


BOOL CDirectSound::SetSoundData(void * pSoundData, DWORD dwSoundSize) {
	LPVOID lpvPtr1;
	DWORD dwBytes1;
	// Obtain write pointer.
	HRESULT hr = m_pDsb->Lock(0, 0, &lpvPtr1, &dwBytes1, 0, 0, DSBLOCK_ENTIREBUFFER);    
    // If DSERR_BUFFERLOST is returned, restore and retry lock.
	if(DSERR_BUFFERLOST == hr) {
		m_pDsb->Restore();
		hr = m_pDsb->Lock(0, 0, &lpvPtr1, &dwBytes1, 0, 0, DSBLOCK_ENTIREBUFFER);
	}
	if(DS_OK == hr) {
		// Write to pointers.
		::CopyMemory(lpvPtr1, pSoundData, dwBytes1);
		// Release the data back to DirectSound.
		hr = m_pDsb->Unlock(lpvPtr1, dwBytes1, 0, 0);
		if(DS_OK == hr)
            return TRUE;
	}
	// Lock, Unlock, or Restore failed.
	return FALSE;
}

void CDirectSound::Play(DWORD dwStartPosition, BOOL bLoop)
{
	if( ! IsValid() || ! IsEnabled() )
		return;		// no chance to play the sound ...

	if( dwStartPosition > m_dwTheSound )
		dwStartPosition = m_dwTheSound;
	m_pDsb->SetCurrentPosition(dwStartPosition);
	if( DSERR_BUFFERLOST == m_pDsb->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0) ) {
		// another application had stolen our buffer
		// Note that a "Restore()" is not enough, because
		// the sound data is invalid after Restore().
		SetSoundData(m_pTheSound, m_dwTheSound);

		// Try playing again
		m_pDsb->Play(0, 0, bLoop ? DSBPLAY_LOOPING : 0);
	}
}

void CDirectSound::Stop()
{
	if( IsValid() )
		m_pDsb->Stop();
}

void CDirectSound::Pause()
{
	Stop();
}

void CDirectSound::Continue()
{
	if( IsValid() ) {
		DWORD dwPlayCursor, dwWriteCursor;
		m_pDsb->GetCurrentPosition(&dwPlayCursor, &dwWriteCursor);
		Play(dwPlayCursor);
	}
}

BOOL CDirectSound::IsValid() const
{
	return (m_lpDirectSound && m_pDsb && m_pTheSound && m_dwTheSound) ? TRUE : FALSE;
}
