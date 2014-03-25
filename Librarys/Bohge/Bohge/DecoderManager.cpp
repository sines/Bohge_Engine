#include "Predefine.h"
#include "DecoderManager.h"
#include "Decoder.h"
#include "SoundResource.h"
#include "LessThread.h"
#include "Utility.h"


using namespace std;

namespace BohgeEngine
{

	DecoderManager* DecoderManager::m_pInstance;

	//-------------------------------------------------------------------------------------------------------
	void DecoderManager::Create()
	{
		m_pInstance = NEW DecoderManager();
	}
	//-------------------------------------------------------------------------------------------------------
	void DecoderManager::Destroy()
	{
		SAFE_DELETE( m_pInstance );
	}
	//-------------------------------------------------------------------------------------------------------
	DecoderManager::DecoderManager(void)
	{
		m_pDecodingLessThread = NEW LessThread();
		m_pDecodingLessThread->SetPriority( Threading::TP_HIGH );
		m_pDecodingLessThread->Start();
	}
	//-------------------------------------------------------------------------------------------------------
	DecoderManager::~DecoderManager(void)
	{
		m_pDecodingLessThread->Stop();
		SAFE_DELETE( m_pDecodingLessThread );
	}
	//-------------------------------------------------------------------------------------------------------
	void DecoderManager::PushDecodeJob( Decoder* job )
	{
		m_pDecodingLessThread->PushJob( job );
	}
	//-------------------------------------------------------------------------------------------------------
	Decoder* DecoderManager::LoadSoundDecoder( const std::string& path )
	{
		Decoder* decoder;
		uint hash = Utility::HashCode( path );
		DecoderReferenceMap::iterator refDecoder = m_DecoderMap.find( hash );
		if ( m_DecoderMap.end() == refDecoder )//没找到解码器，创建新的解码器
		{
			decoder = Decoder::DecoderFactory( path );
			DecoderReference* dr = NEW DecoderReference( decoder );
			m_DecoderMap.insert( make_pair( hash, dr ) );
		}
		else
		{
			refDecoder->second->m_nReference ++;
			decoder = refDecoder->second->m_pDecoder;
		}
		return decoder;
	}
	//-------------------------------------------------------------------------------------------------------
	void DecoderManager::UnloadSoundDecoder( Decoder* sr )
	{
		uint hash = Utility::HashCode( sr->GetFilePath() );
		DecoderReferenceMap::iterator refDecoder = m_DecoderMap.find( hash );
		ASSERT( m_DecoderMap.end() != refDecoder );
		if ( 0 == -- refDecoder->second->m_nReference )
		{
			Decoder::DestoryDecoder( refDecoder->second->m_pDecoder );
			SAFE_DELETE( refDecoder->second );
			m_DecoderMap.erase( refDecoder );
		}
	}
}
