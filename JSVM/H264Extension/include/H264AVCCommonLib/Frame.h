/*
********************************************************************************

NOTE - One of the two copyright statements below may be chosen
       that applies for the software.

********************************************************************************

This software module was originally developed by

Heiko Schwarz    (Fraunhofer HHI),
Tobias Hinz      (Fraunhofer HHI),
Karsten Suehring (Fraunhofer HHI)

in the course of development of the ISO/IEC 14496-10:2005 Amd.1 (Scalable Video
Coding) for reference purposes and its performance may not have been optimized.
This software module is an implementation of one or more tools as specified by
the ISO/IEC 14496-10:2005 Amd.1 (Scalable Video Coding).

Those intending to use this software module in products are advised that its
use may infringe existing patents. ISO/IEC have no liability for use of this
software module or modifications thereof.

Assurance that the originally developed software module can be used
(1) in the ISO/IEC 14496-10:2005 Amd.1 (Scalable Video Coding) once the
ISO/IEC 14496-10:2005 Amd.1 (Scalable Video Coding) has been adopted; and
(2) to develop the ISO/IEC 14496-10:2005 Amd.1 (Scalable Video Coding): 

To the extent that Fraunhofer HHI owns patent rights that would be required to
make, use, or sell the originally developed software module or portions thereof
included in the ISO/IEC 14496-10:2005 Amd.1 (Scalable Video Coding) in a
conforming product, Fraunhofer HHI will assure the ISO/IEC that it is willing
to negotiate licenses under reasonable and non-discriminatory terms and
conditions with applicants throughout the world.

Fraunhofer HHI retains full right to modify and use the code for its own
purpose, assign or donate the code to a third party and to inhibit third
parties from using the code for products that do not conform to MPEG-related
ITU Recommendations and/or ISO/IEC International Standards. 

This copyright notice must be included in all copies or derivative works.
Copyright (c) ISO/IEC 2005. 

********************************************************************************

COPYRIGHT AND WARRANTY INFORMATION

Copyright 2005, International Telecommunications Union, Geneva

The Fraunhofer HHI hereby donate this source code to the ITU, with the following
understanding:
    1. Fraunhofer HHI retain the right to do whatever they wish with the
       contributed source code, without limit.
    2. Fraunhofer HHI retain full patent rights (if any exist) in the technical
       content of techniques and algorithms herein.
    3. The ITU shall make this code available to anyone, free of license or
       royalty fees.

DISCLAIMER OF WARRANTY

These software programs are available to the user without any license fee or
royalty on an "as is" basis. The ITU disclaims any and all warranties, whether
express, implied, or statutory, including any implied warranties of
merchantability or of fitness for a particular purpose. In no event shall the
contributor or the ITU be liable for any incidental, punitive, or consequential
damages of any kind whatsoever arising from the use of these programs.

This disclaimer of warranty extends to the user of these programs and user's
customers, employees, agents, transferees, successors, and assigns.

The ITU does not represent or warrant that the programs furnished hereunder are
free of infringement of any third-party patents. Commercial implementations of
ITU-T Recommendations, including shareware, may be subject to royalty fees to
patent holders. Information regarding the ITU-T patent policy is available from 
the ITU Web site at http://www.itu.int.

THIS IS NOT A GRANT OF PATENT RIGHTS - SEE THE ITU-T PATENT POLICY.

********************************************************************************
*/




#if !defined(AFX_INTFRAME_H__98AFB9AC_5EE3_45A9_B09B_859511AC9090__INCLUDED_)
#define AFX_INTFRAME_H__98AFB9AC_5EE3_45A9_B09B_859511AC9090__INCLUDED_



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "H264AVCCommonLib/YuvPicBuffer.h"
#include "ResizeParameters.h"

H264AVC_NAMESPACE_BEGIN


class DownConvert;
class QuarterPelFilter;
class MbDataCtrl;
class DPBUnit;
class RecPicBufUnit;
class ReconstructionBypass;

class H264AVCCOMMONLIB_API Frame
{
public:
	Frame                ( YuvBufferCtrl&    rcYuvFullPelBufferCtrl,
                         YuvBufferCtrl&    rcYuvHalfPelBufferCtrl,
												 PicType           ePicType,
                         Frame*            pcFrame );
	virtual ~Frame       ();

  ErrVal  init            ();
  ErrVal  initHalfPel     ();
  ErrVal  initHalfPel     ( XPel*& rpucYuvBuffer );

  static ErrVal create    ( Frame*& rpcIntFrame, YuvBufferCtrl& rcYuvFullPelBufferCtrl, YuvBufferCtrl& rcYuvHalfPelBufferCtrl, PicType ePicType, Frame* pcFrame );
	ErrVal  destroy         ();
  ErrVal  uninit          ();
  ErrVal  uninitHalfPel   ();

  ErrVal  load            ( PicBuffer*        pcPicBuffer );
  ErrVal  store           ( PicBuffer*        pcPicBuffer, PicType ePicType = FRAME );
  
  ErrVal  addFrameFieldBuffer   ();
	ErrVal  removeFrameFieldBuffer();
  ErrVal  addFieldBuffer        ( PicType ePicType );
  ErrVal  removeFieldBuffer     ( PicType ePicType );
  ErrVal  extendFrame           ( QuarterPelFilter* pcQuarterPelFilter, PicType ePicType=FRAME, Bool bFrameMbsOnlyFlag=true );

  Void      setDPBUnit      ( DPBUnit*  pcDPBUnit ) { m_pcDPBUnit = pcDPBUnit; }
  DPBUnit*  getDPBUnit      ()                      { return m_pcDPBUnit; }
  Bool      isLongTerm  ()          const { return m_bLongTerm; }
  Void      setLongTerm ( Bool b )     
  {
    m_bLongTerm = b; 
    if( NULL != m_pcFrameTopField ) m_pcFrameTopField->setLongTerm( m_bLongTerm );
    if( NULL != m_pcFrameBotField ) m_pcFrameBotField->setLongTerm( m_bLongTerm );
  }

  const PictureParameters&  getPicParameters()                    const;
  const PictureParameters&  getPicParameters( PicType ePicType )  const;
  ErrVal  setPicParameters  ( const ResizeParameters&   rcRP,     const SliceHeader* pcSH     = 0 );
  ErrVal  setPicParameters  ( const PictureParameters&  rcPP,     PicType            ePicType = FRAME );
  ErrVal  copyPicParameters ( const Frame&              rcFrame,  PicType            ePicType = FRAME );

  const Frame*  getFrame() const { return m_pcFrame; }

  Void            setRecPicBufUnit( RecPicBufUnit* pcUnit ) { m_pcDPBUnit = (DPBUnit*)(Void*)pcUnit; }
  RecPicBufUnit*  getRecPicBufUnit()                        { return (RecPicBufUnit*)(Void*)m_pcDPBUnit; }
  ErrVal clip()
  {
    ASSERT( m_ePicType==FRAME );
    RNOK( getFullPelYuvBuffer()->clip() );
    return Err::m_nOK;
  }
  
  ErrVal prediction       ( Frame* pcMCPFrame, Frame* pcSrcFrame, PicType ePicType )
  {
    ASSERT( m_ePicType==FRAME );
    
    if( ePicType==FRAME )
  {
      RNOK( getFullPelYuvBuffer()->prediction( pcSrcFrame->getFullPelYuvBuffer(), 
                                               pcMCPFrame->getFullPelYuvBuffer() ) );
    }
    else
  {
			RNOK(             addFieldBuffer( ePicType ) );
			RNOK( pcSrcFrame->addFieldBuffer( ePicType ) );
			RNOK( pcMCPFrame->addFieldBuffer( ePicType ) );
      RNOK( getPic( ePicType )->getFullPelYuvBuffer()->prediction( pcSrcFrame->getPic( ePicType )->getFullPelYuvBuffer(),
                                                                   pcMCPFrame->getPic( ePicType )->getFullPelYuvBuffer() ) );

    }
    return Err::m_nOK;
  }
  
  ErrVal update           ( Frame* pcMCPFrame, Frame* pcSrcFrame, UInt uiShift )
  {
    ASSERT( m_ePicType==FRAME );
    RNOK( getFullPelYuvBuffer()->update           ( pcSrcFrame->getFullPelYuvBuffer(), pcMCPFrame->getFullPelYuvBuffer(), uiShift ) );
    return Err::m_nOK;
  }
  
  ErrVal inverseUpdate    ( Frame* pcMCPFrame, Frame* pcSrcFrame, UInt uiShift )
  {
    ASSERT( m_ePicType==FRAME );
    RNOK( getFullPelYuvBuffer()->inverseUpdate    ( pcSrcFrame->getFullPelYuvBuffer(), pcMCPFrame->getFullPelYuvBuffer(), uiShift ) );
    return Err::m_nOK;
  }


  ErrVal update           ( Frame* pcMCPFrame0, Frame* pcMCPFrame1, Frame* pcSrcFrame )
  {
    ASSERT( m_ePicType==FRAME );
    RNOK( getFullPelYuvBuffer()->update           ( pcSrcFrame->getFullPelYuvBuffer(), pcMCPFrame0->getFullPelYuvBuffer(), pcMCPFrame1->getFullPelYuvBuffer() ) );
    return Err::m_nOK;
  }
  
  ErrVal inverseUpdate    ( Frame* pcMCPFrame0, Frame* pcMCPFrame1, Frame* pcSrcFrame )
  {
    ASSERT( m_ePicType==FRAME );
		if (pcMCPFrame0 && pcMCPFrame1){
			RNOK( getFullPelYuvBuffer()->inverseUpdate    ( pcSrcFrame->getFullPelYuvBuffer(), pcMCPFrame0->getFullPelYuvBuffer(), pcMCPFrame1->getFullPelYuvBuffer() ) );
		}else if (pcMCPFrame0){
			RNOK( getFullPelYuvBuffer()->inverseUpdate    ( pcSrcFrame->getFullPelYuvBuffer(), pcMCPFrame0->getFullPelYuvBuffer(), (YuvPicBuffer*)NULL ) );
		}else{
			RNOK( getFullPelYuvBuffer()->inverseUpdate    ( pcSrcFrame->getFullPelYuvBuffer(), (YuvPicBuffer*)NULL, pcMCPFrame1->getFullPelYuvBuffer() ) );
		}
    return Err::m_nOK;
  }

  ErrVal inversePrediction( Frame* pcMCPFrame,  Frame* pcSrcFrame, PicType ePicType )
  {
    ASSERT( m_ePicType==FRAME );

    if( ePicType==FRAME )
  {
      RNOK( getFullPelYuvBuffer()->inversePrediction( pcSrcFrame->getFullPelYuvBuffer(), 
                                                      pcMCPFrame->getFullPelYuvBuffer() ) );
    }
    else
  {
			RNOK(             addFieldBuffer( ePicType ) );
			RNOK( pcSrcFrame->addFieldBuffer( ePicType ) );
			RNOK( pcMCPFrame->addFieldBuffer( ePicType ) );
      RNOK( getPic( ePicType )->getFullPelYuvBuffer()->inversePrediction( pcSrcFrame->getPic( ePicType )->getFullPelYuvBuffer(),
                                                                          pcMCPFrame->getPic( ePicType )->getFullPelYuvBuffer() ) );

    }
    return Err::m_nOK;
  }


  ErrVal  copyAll     ( Frame* pcSrcFrame )
  {
    ASSERT( m_ePicType==FRAME );
    m_iPoc          = pcSrcFrame->m_iPoc;
    m_iTopFieldPoc  = pcSrcFrame->m_iTopFieldPoc;
    m_iBotFieldPoc  = pcSrcFrame->m_iBotFieldPoc;
    RNOK( getFullPelYuvBuffer()->copy( pcSrcFrame->getFullPelYuvBuffer() ) );
  
    return Err::m_nOK;
  }

  ErrVal copy             ( Frame* pcSrcFrame, PicType ePicType )
  {
    ASSERT( m_ePicType==FRAME );
    if( ePicType==FRAME )
    {
      RNOK( getFullPelYuvBuffer()->copy( pcSrcFrame->getFullPelYuvBuffer()) );
    }
    else
    {
      RNOK(             addFieldBuffer( ePicType ) );
      RNOK( pcSrcFrame->addFieldBuffer( ePicType ) );
      RNOK( getPic( ePicType )->getFullPelYuvBuffer()->copy( pcSrcFrame->getPic( ePicType )->getFullPelYuvBuffer() ) );
    }
    return Err::m_nOK;
  }
  
    //JVT-X046 {
  ErrVal predictionSlices       (Frame* pcSrcFrame,Frame* pcMCPFrame, UInt uiMbY, UInt uiMbX)
  {	 
	  RNOK( getFullPelYuvBuffer()->predictionSlices( pcSrcFrame->getFullPelYuvBuffer(), pcMCPFrame->getFullPelYuvBuffer(), uiMbY, uiMbX ) );
	  return Err::m_nOK;
  }
  ErrVal inversepredictionSlices       (Frame* pcSrcFrame,Frame* pcMCPFrame, UInt uiMbY, UInt uiMbX)
  {	 
	  RNOK( getFullPelYuvBuffer()->inversepredictionSlices( pcSrcFrame->getFullPelYuvBuffer(), pcMCPFrame->getFullPelYuvBuffer(), uiMbY, uiMbX ) );
	  return Err::m_nOK;
  }
  ErrVal copyMb       (Frame* pcSrcFrame, UInt uiMbY ,UInt uiMbX)
  {	 
	  RNOK( getFullPelYuvBuffer()->copyMb( pcSrcFrame->getFullPelYuvBuffer(), uiMbY, uiMbX ) );
	  return Err::m_nOK;
  }
	void   setMBZero( UInt uiMBY, UInt uiMBX ) { getFullPelYuvBuffer()->setMBZero(uiMBY,uiMBX); }
	ErrVal copySlice (Frame* pcSrcFrame, PicType ePicType, UInt uiFirstMB, UInt uiLastMB)
  {
	  ASSERT( m_ePicType==FRAME );
	  if( ePicType==FRAME )
	  {
		  RNOK( getFullPelYuvBuffer()->copySlice( pcSrcFrame->getFullPelYuvBuffer(),uiFirstMB,uiLastMB) );
	  }
	  return Err::m_nOK;
  }
	//JVT-X046 }

  ErrVal  subtract    ( Frame* pcSrcFrame0, Frame* pcSrcFrame1 )
  {
    ASSERT( m_ePicType==FRAME );
    RNOK( getFullPelYuvBuffer()->subtract( pcSrcFrame0->getFullPelYuvBuffer(), pcSrcFrame1->getFullPelYuvBuffer() ) );
    return Err::m_nOK;
  }
  
  ErrVal add              ( Frame* pcSrcFrame,  PicType ePicType )
  {
    ASSERT( m_ePicType==FRAME );

    if( ePicType==FRAME )
  {
    RNOK( getFullPelYuvBuffer()->add ( pcSrcFrame->getFullPelYuvBuffer()) );
    }
    else
    {
			RNOK(             addFieldBuffer( ePicType ) );
			RNOK( pcSrcFrame->addFieldBuffer( ePicType ) );
      RNOK( getPic( ePicType )->getFullPelYuvBuffer()->add( pcSrcFrame->getPic( ePicType )->getFullPelYuvBuffer() ) );
    }
    return Err::m_nOK;
  }
  
  ErrVal  setZero     ()
  {
    ASSERT( m_ePicType==FRAME );
    getFullPelYuvBuffer()->setZero();
    return Err::m_nOK;
  }

  ErrVal  setNonZeroFlags( UShort* pusNonZeroFlags, UInt uiStride )
  {
    ASSERT( m_ePicType==FRAME );
    return getFullPelYuvBuffer()->setNonZeroFlags( pusNonZeroFlags, uiStride );
  }

  ErrVal getSSD( Double& dSSDY, Double& dSSDU, Double& dSSDV, PicBuffer* pcOrgPicBuffer )
  {
    ASSERT( m_ePicType==FRAME );
    RNOK( m_cFullPelYuvBuffer.getSSD( dSSDY, dSSDU, dSSDV, pcOrgPicBuffer ) );
    return Err::m_nOK;
  }

  ErrVal dump( FILE* pFile, Int uiBandType, MbDataCtrl* pcMbDataCtrl )
  {
    if( uiBandType != 0 )
    {
      RNOK( getFullPelYuvBuffer()->dumpHPS( pFile, pcMbDataCtrl ) );
    }
    else
    {
      RNOK( getFullPelYuvBuffer()->dumpLPS( pFile ) );
    }
		fflush( pFile );
    return Err::m_nOK;
  }

  ErrVal  intraUpsampling   ( Frame*                pcBaseFrame, 
                              Frame*                pcTempBaseFrame,
                              Frame*                pcTempFrame,
                              DownConvert&          rcDownConvert,
                              ResizeParameters*     pcParameters,
                              MbDataCtrl*           pcMbDataCtrlBase, 
                              MbDataCtrl*           pcMbDataCtrlPredFrm, 
                              MbDataCtrl*           pcMbDataCtrlPredFld, 
                              ReconstructionBypass* pcReconstructionBypass,
                              Bool                  bConstrainedIntraUpsamplingFlag,
                              Bool*                 pabBaseModeAllowedFlagArrayFrm = 0, 
                              Bool*                 pabBaseModeAllowedFlagArrayFld = 0 );
  ErrVal residualUpsampling ( Frame*                pcBaseFrame,
                              DownConvert&          rcDownConvert,
                              ResizeParameters*     pcParameters,
                              MbDataCtrl*           pcMbDataCtrlBase );

  YuvPicBuffer*  getFullPelYuvBuffer     ()        { return &m_cFullPelYuvBuffer; }
  YuvPicBuffer*  getHalfPelYuvBuffer     ()        { return &m_cHalfPelYuvBuffer; }

	Bool  isPocAvailable()           const { return m_bPocIsSet; }
	Int   getPoc        ()           const { return m_iPoc; }
  Int   getTopFieldPoc()           const { return m_iTopFieldPoc; }
  Int   getBotFieldPoc()           const { return m_iBotFieldPoc; }

  Void  clearPoc      ()
  {
    if( m_pcFrameTopField )
    {
      m_pcFrameTopField->clearPoc();
    }
    if( m_pcFrameBotField )
    {
      m_pcFrameBotField->clearPoc();
    }
    m_iTopFieldPoc  = MSYS_INT_MIN;
    m_iBotFieldPoc  = MSYS_INT_MIN;
    m_iPoc          = MSYS_INT_MIN;
    m_bPocIsSet     = false;
  }

	Void  setPoc        ( Int iPoc )       { m_iPoc = iPoc; m_bPocIsSet = true; }
	Void  setPoc        ( const SliceHeader& rcSH )
	{ 
		ASSERT( m_ePicType==FRAME );
		const PicType ePicType = rcSH.getPicType();

    if( ePicType & TOP_FIELD )
    {
      m_iTopFieldPoc = rcSH.getTopFieldPoc();
      if( m_pcFrameTopField && m_pcFrameBotField )
      {
        m_pcFrameTopField->setPoc( m_iTopFieldPoc );
        setPoc( m_pcFrameBotField->isPocAvailable() ? max( m_pcFrameBotField->getPoc(), m_iTopFieldPoc ) : m_iTopFieldPoc );
      }
    }
    if( ePicType & BOT_FIELD )
    {
      m_iBotFieldPoc = rcSH.getBotFieldPoc();
      if( m_pcFrameTopField && m_pcFrameBotField )
      {
        m_pcFrameBotField->setPoc( m_iBotFieldPoc );
        setPoc( m_pcFrameTopField->isPocAvailable() ? min( m_pcFrameTopField->getPoc(), m_iBotFieldPoc ) : m_iBotFieldPoc );
      }
    }
    if( ! m_pcFrameTopField || ! m_pcFrameBotField )
    {
      setPoc( max( m_iTopFieldPoc, m_iBotFieldPoc ) );
    }
  }

  Bool  isHalfPel()   { return m_bHalfPel; }

  const Frame*  getPic( PicType ePicType ) const;
  Frame*        getPic( PicType ePicType );

	PicType getPicType ()            const { return m_ePicType; }
	Void setPicType    ( PicType ePicType ){ m_ePicType = ePicType; }

  Bool  isExtended () { return m_bExtended; }
  Void  clearExtended() { m_bExtended = false; }
  Void  setExtended  ()                  { m_bExtended = true; }

  // JVT-R057 LA-RDO{
  Void   initChannelDistortion();
  Void   uninitChannelDistortion()  { 
	  if(m_piChannelDistortion) 
		  delete[] m_piChannelDistortion; 
  }
  UInt*   getChannelDistortion()   { return  m_piChannelDistortion;}
  Void   copyChannelDistortion(Frame*p1);
  Void   zeroChannelDistortion();
  Void   setChannelDistortion(Frame*p1) { if(p1) m_piChannelDistortion=p1->m_piChannelDistortion; else m_piChannelDistortion=NULL;}
  // JVT-R057 LA-RDO}  
  
  Void  setUnvalid()  { m_bUnvalid = true;  }
  Void  setValid  ()  { m_bUnvalid = false; }
  Bool  isUnvalid ()  { return m_bUnvalid;  }

protected:
  ErrVal  xUpdatePicParameters();

protected:
  YuvPicBuffer      m_cFullPelYuvBuffer;
  YuvPicBuffer      m_cHalfPelYuvBuffer;
  PictureParameters m_cPicParameters;     // for frame or top field
  PictureParameters m_cPicParametersBot;  // for bottom field
  
  Bool            m_bHalfPel;
  Bool            m_bExtended;
	Bool            m_bPocIsSet;
  Int             m_iTopFieldPoc;
  Int             m_iBotFieldPoc;
  Int             m_iPoc;
	PicType         m_ePicType;
  Frame*          m_pcFrameTopField;
  Frame*          m_pcFrameBotField;
  Frame*          m_pcFrame;

  DPBUnit*        m_pcDPBUnit;
  // JVT-R057 LA-RDO{
  UInt*            m_piChannelDistortion;
  // JVT-R057 LA-RDO}

  Bool      m_bLongTerm;
  Bool      m_bUnvalid;
};

H264AVCCOMMONLIB_API extern __inline ErrVal gSetFrameFieldLists ( RefFrameList& rcTopFieldList, RefFrameList& rcBotFieldList, RefFrameList& rcRefFrameList )
{
  ROTRS( NULL == &rcRefFrameList, Err::m_nOK );
 
  rcTopFieldList.reset();
  rcBotFieldList.reset();

  const Int iMaxEntries = min( rcRefFrameList.getSize(), rcRefFrameList.getActive() );
  for( Int iFrmIdx = 0; iFrmIdx < iMaxEntries; iFrmIdx++ )
  {
    Frame* pcTopField = rcRefFrameList.getEntry( iFrmIdx )->getPic( TOP_FIELD );
    Frame* pcBotField = rcRefFrameList.getEntry( iFrmIdx )->getPic( BOT_FIELD );
    rcTopFieldList.add( pcTopField );
    rcTopFieldList.add( pcBotField );
    rcBotFieldList.add( pcBotField );
    rcBotFieldList.add( pcTopField );
  }

  return Err::m_nOK;
}



H264AVCCOMMONLIB_API extern __inline ErrVal gSetFrameFieldArrays( Frame* apcFrame[4], Frame* pcFrame )
{
  if( pcFrame == NULL )
  {
    apcFrame[0] = NULL;
    apcFrame[1] = NULL;
    apcFrame[2] = NULL;
    apcFrame[3] = NULL;
  }
  else
  {
		RNOK( pcFrame->addFrameFieldBuffer() );
    apcFrame[0] = pcFrame->getPic( TOP_FIELD );
    apcFrame[1] = pcFrame->getPic( BOT_FIELD );
    apcFrame[2] = pcFrame->getPic( FRAME     );
    apcFrame[3] = pcFrame->getPic( FRAME     );
  }
  return Err::m_nOK;
}

H264AVC_NAMESPACE_END


#endif // !defined(AFX_INTFRAME_H__98AFB9AC_5EE3_45A9_B09B_859511AC9090__INCLUDED_)
