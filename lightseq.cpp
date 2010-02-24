// LightSeq.cpp : Implementation of CVBATestApp and DLL registration.

#include "stdafx.h"
#include "VBATest.h"
#include "main.h"
#include <atlbase.h>

/////////////////////////////////////////////////////////////////////////////
//

LightSeq::LightSeq() : m_LightSeqCenter(this)
{
	m_ptu = NULL;
}

LightSeq::~LightSeq()
{
}

HRESULT LightSeq::Init(PinTable *ptable, float x, float y)
{
	m_ptable = ptable;

	m_d.m_v.x = x;
	m_d.m_v.y = y;

	SetDefaults();

	return InitVBA(fTrue, 0, NULL);
}

void LightSeq::SetDefaults()
{
	m_d.m_updateinterval		= 25;
	m_d.m_wzCollection[0] 		= 0x00;
	m_d.m_vCenter.x				= 1000/2;
	m_d.m_vCenter.y				= 2000/2;
	m_d.m_tdr.m_fTimerEnabled	= fFalse;
	m_d.m_tdr.m_TimerInterval	= 100;
}

void LightSeq::SetObjectPos()
{
	g_pvp->SetObjectPosCur(m_d.m_v.x, m_d.m_v.y);
}

void LightSeq::MoveOffset(float dx, float dy)
{
	m_d.m_v.x += dx;
	m_d.m_v.y += dy;

	m_ptable->SetDirtyDraw();
}

void LightSeq::GetCenter(Vertex *pv)
{
	pv->x = m_d.m_v.x;
	pv->y = m_d.m_v.y;
}

void LightSeq::PutCenter(Vertex *pv)
{
	m_d.m_v.x = pv->x;
	m_d.m_v.y = pv->y;

	m_ptable->SetDirtyDraw();
}

// this function draws the shape of the object with a solid fill
// only used in the editor and not the game
//
// this is called before the grid lines are drawn on the map
//
void LightSeq::PreRender(Sur *psur)
{
	int		i;
	double	angle;
	float	sn,cs;

	psur->SetBorderColor(RGB(0,0,0),fFalse,0);
	psur->SetObject(this);

	for (i=0;i<8;i++)
	{
		if (i % 2 == 0)
		{
			psur->SetFillColor(RGB(255,0,0));
		}
		else
		{
			psur->SetFillColor(RGB(128,0,0));
		}
		angle = ((PI*2)/8)*i;
		sn = (float)sin(angle);
		cs = (float)cos(angle);
		psur->Ellipse(m_d.m_v.x + sn*12, m_d.m_v.y - cs*12, 4);
	}

	psur->SetFillColor(RGB(255,0,0));
	psur->Ellipse(m_d.m_v.x, m_d.m_v.y-3, 4);
}

// this function draws the shape of the object with a black outline (no solid fill)
// only used in the editor and not the game
//
// this is called after the grid lines have been drawn on the map.  draws a solid
// outline over the grid lines
//
void LightSeq::Render(Sur *psur)
{
	int		i;
	double	angle;
	float	sn,cs;

	psur->SetFillColor(-1);
	psur->SetBorderColor(RGB(0,0,0),fFalse,0);

	psur->SetObject(this);

	psur->Ellipse(m_d.m_v.x, m_d.m_v.y, 18);

	for (i=0;i<8;i++)
	{
		angle = ((PI*2)/8)*i;
		sn = (float)sin(angle);
		cs = (float)cos(angle);
		psur->Ellipse(m_d.m_v.x + sn*12, m_d.m_v.y - cs*12, 4);
	}

	psur->Ellipse(m_d.m_v.x, m_d.m_v.y-3, 4);

	RenderOutline(psur);
}

// this function draw the little center marker which is a cross with the usual LS circles on it

void LightSeq::RenderOutline(Sur *psur)
{
	int		i;
	double	angle;
	float	sn,cs;

	psur->SetBorderColor(RGB(0,0,0),fFalse,0);
	psur->SetObject((ISelect *)&m_LightSeqCenter);

	psur->Line(m_d.m_vCenter.x - 10, m_d.m_vCenter.y, m_d.m_vCenter.x + 10, m_d.m_vCenter.y);
	psur->Line(m_d.m_vCenter.x, m_d.m_vCenter.y - 10, m_d.m_vCenter.x, m_d.m_vCenter.y + 10);

	for (i=0;i<8;i++)
	{
		if (i % 2 == 0)
		{
			psur->SetFillColor(RGB(255,0,0));
		}
		else
		{
			psur->SetFillColor(RGB(128,0,0));
		}
		angle = ((PI*2)/8)*i;
		sn = (float)sin(angle);
		cs = (float)cos(angle);
		psur->Ellipse(m_d.m_vCenter.x + sn*7, m_d.m_vCenter.y - cs*7, 2);
	}

	psur->SetFillColor(RGB(255,0,0));
	psur->Ellipse(m_d.m_vCenter.x, m_d.m_vCenter.y-2.5f, 2);
}

// Renders the image onto the Blueprint
//
// We don't want this on the blue print as it is non-essensial
//
void LightSeq::RenderBlueprint(Sur *psur)
{
}

// Registers the timer with the game call which then makes a call back when the interval
// has expired.
//
// for this sort of object it is basically not really required but hey, somebody might use it..
//
void LightSeq::GetTimers(Vector<HitTimer> *pvht)
{
	HitTimer *pht;
	pht = new HitTimer();
	pht->m_interval = m_d.m_tdr.m_TimerInterval;
	pht->m_nextfire = pht->m_interval;
	pht->m_pfe = (IFireEvents *)this;

	m_phittimer = pht;

	if (m_d.m_tdr.m_fTimerEnabled)
	{
		pvht->AddElement(pht);
	}
}

// This function is supposed to return the hit shapes for the object but since it is
// off screen we use it to register the screen updater in the game engine.. this means
// that Check3d (and Draw3d) are called in the updater class.
//
void LightSeq::GetHitShapes(Vector<HitObject> *pvho)
{
    m_ptu = new LightSeqUpdater(this);

	m_ptu->m_lightseqanim.m_znear = 0;
	m_ptu->m_lightseqanim.m_zfar = 0;

	// HACK - adding object directly to screen update list.  Someday make hit objects and screenupdaters seperate objects
	g_pplayer->m_vscreenupdate.AddElement(&m_ptu->m_lightseqanim);
}

void LightSeq::GetHitShapesDebug(Vector<HitObject> *pvho)
	{
	}

// This method is called as the game exits..
// it cleans up any allocated memory used by the instace of the object
//
void LightSeq::EndPlay()
{
	if (m_pgridData != NULL)
	{
		delete m_pgridData;
	}

	if (m_ptu != NULL)
	{
		delete m_ptu;
		m_ptu = NULL;
	}

	IEditable::EndPlay();
}

void LightSeq::RenderStatic(LPDIRECT3DDEVICE7 pd3dDevice)
{
}

void LightSeq::RenderMoversFromCache(Pin3D *ppin3d)
{
	RenderMovers(ppin3d->m_pd3dDevice);
}

void LightSeq::RenderMovers(LPDIRECT3DDEVICE7 pd3dDevice)
{
	long 			i;
	long			size;
	CComBSTR 		bstr;
	CComBSTR 		bstrCollection;
	ItemTypeEnum	type;
	Light			*pLight;
	Bumper			*pBumper;
	float			x, y;
	int				ix, iy;

	// zero pointers as a safe guard
	m_pcollection	= NULL;
	m_pgridData	= NULL;
	// no animation in progress
	m_playInProgress  = false;
	m_pauseInProgress = false;
	// turn off any tracers
	m_th1.type = eSeqNull;
	m_th2.type = eSeqNull;
	m_tt1.type = eSeqNull;
	m_tt2.type = eSeqNull;
	// flush the queue
	m_queue.Head = 0;
	m_queue.Tail = 0;

	// get a BSTR version of the collection we are to use
	bstrCollection = m_d.m_wzCollection;

	// get the number of collections available
	size = m_ptable->m_vcollection.Size();
	for(i=0; i<size; i++)
	{
		// get the name of this collection
		m_ptable->m_vcollection.ElementAt(i)->get_Name(&bstr);
		// is it the one we are to use?
		if (WideStrCmp(bstr, bstrCollection) == 0)
		{
			// yep, set a pointer to this sub-collection
			m_pcollection = m_ptable->m_vcollection.ElementAt(i);
			break;
		}
	}

	// if the collection wasn't found or there are no collections available then bomb out
	if (m_pcollection == NULL)
	{
		return;
	}

	// get the grid demensions (from the table size)
	float tablewidth = m_ptable->m_right - m_ptable->m_left;
	float tableheight = m_ptable->m_bottom - m_ptable->m_top;

	m_lightSeqGridWidth  = (int)tablewidth / LIGHTSEQGRIDSCALE;
	m_lightSeqGridHeight = (int)tableheight / LIGHTSEQGRIDSCALE;

	// set the centre point of the grid for effects which start from the center
	m_GridXCenter = (float)((int)(m_d.m_vCenter.x / LIGHTSEQGRIDSCALE));
	m_GridYCenter = (float)((int)(m_d.m_vCenter.y / LIGHTSEQGRIDSCALE));
	m_GridXCenterAdjust	= abs(m_lightSeqGridWidth/2 - (int)m_GridXCenter);
	m_GridYCenterAdjust = abs(m_lightSeqGridHeight/2 - (int)m_GridYCenter);

	// allocate the grid for this sequence
	m_pgridData = new short[m_lightSeqGridHeight*m_lightSeqGridWidth];
	if (m_pgridData == NULL)
	{
		// make the entire collection (for the sequencer) invalid and bomb out
		m_pcollection = NULL;
		return;
	}
	else
	{
		memset ((void *)m_pgridData, 0x00, (size_t)((m_lightSeqGridHeight*m_lightSeqGridWidth)*sizeof(short)));
	}

	// get the number of elements (objects) in the collection (referenced by m_visel)
	size = m_pcollection->m_visel.Size();

	// go though the collection and get the cordinates of all the lights and bumper
	for(i=0; i<size; i++)
	{
		// get the type of object
		type = m_pcollection->m_visel.ElementAt(i)->GetIEditable()->GetItemType();
		// must be a light or bumper
		if ( (type == eItemLight) || (type == eItemBumper) )
		{
			if (type == eItemLight)
			{
				// process a light
				pLight = (Light *)m_pcollection->m_visel.ElementAt(i);
				pLight->get_X(&x);
				pLight->get_Y(&y);

				if (pLight->m_fBackglass == fTrue)
				{
					// if the light is on the backglass then scale up its Y position
					y = y * 2.666f; // 2 little devils ;-)
				}
			}
			else
			{
				// process a bumper
				pBumper = (Bumper *)m_pcollection->m_visel.ElementAt(i);
				pBumper->get_X(&x);
				pBumper->get_Y(&y);
			}

			// scale down to suit the size of the light sequence grid
			ix = (int)(x / LIGHTSEQGRIDSCALE);
			iy = (int)(y / LIGHTSEQGRIDSCALE);
			// if on the playfield (1000 by 2000)
			if ( ((ix >= 0) && (ix < m_lightSeqGridWidth)) &&
				 ((iy >= 0) && (iy < m_lightSeqGridHeight)) )
			{
				int	gridIndex = (iy * m_lightSeqGridWidth) + ix;

				// then store the index offset into the grid (plus 1, 0 is no object)
				m_pgridData[gridIndex] = (short)i + 1;
			}
		}
	}
}

// This function is called during Check3D.  It basically check to see if the update
// interval has expired and if so handles the light effect
//
// if a screen update is required it returns true.. but since the light sequencer doesn't
// have a screen update it always returns false.
//
bool LightSeq::RenderAnimation()
{
	bool	th1finished, th2finished, tt1finished, tt2finished;
	int		Tail;

	if (m_playInProgress == true)
	{
	    if (g_pplayer->m_timeCur >= m_timeNextUpdate)
		{
			if (m_pauseInProgress == false)
			{
	    	    m_timeNextUpdate = g_pplayer->m_timeCur + m_updateRate;
				// process the head tracers
				th1finished = ProcessTracer(&m_th1, LightStateOn);
				th2finished = ProcessTracer(&m_th2, LightStateOn);
				// and any tail tracers
				tt1finished = ProcessTracer(&m_tt1, LightStateOff);
				tt2finished = ProcessTracer(&m_tt2, LightStateOff);

				// has the animation finished
				if ( (th1finished == true) &&
					 (th2finished == true) &&
					 (tt1finished == true) &&
					 (tt2finished == true) )
				{
					// the sequence has finished, paused for the specified value or a single interval
					// (bit of a breather or load balance)
					m_timeNextUpdate = g_pplayer->m_timeCur + max(m_updateRate, m_pauseValue);
					m_pauseInProgress = true;
				}
			}
			else
			{
				// test the remaining replays?
				m_replayCount--;
				if (m_replayCount != 0)
				{
					// if not zero then restart the same animation again
					SetupTracers(m_playAnimation, m_tailLength, m_replayCount, m_pauseValue);
				}
				else
				{
					// move the tail to the next position
					Tail = m_queue.Tail + 1;
					if (Tail >= LIGHTSEQQUEUESIZE)
	   	   			{
	   	   				Tail = 0;
	   	   			}
					m_queue.Tail = Tail;
					// not playing at the moment
					m_playInProgress = false;
					// if the queue is empty then reset the lights to their real state
					if (m_queue.Head == m_queue.Tail)
					{
						StopPlay();
						// and signal the script ( Sub <LIGHTSEQNAME>_PlayDone() )
						FireVoidEvent(DISPID_LightSeqEvents_PlayDone);
					}
				}
			}
		}
	}
	else
	{
		// is there something in the queue?
		if (m_queue.Head != m_queue.Tail)
		{
			// yes
			Tail = m_queue.Tail;
			// set the update rate for this sequence
			m_updateRate = m_queue.Data[Tail].UpdateRate;
			// set up the tracers
			SetupTracers(m_queue.Data[Tail].Animation,
						 m_queue.Data[Tail].TailLength,
						 m_queue.Data[Tail].Repeat,
						 m_queue.Data[Tail].Pause);
			// and start the ball rolling again
			m_playInProgress = true;
		}
	}

	return (false);
}



STDMETHODIMP LightSeq::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_ILightSeq,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

HRESULT LightSeq::SaveData(IStream *pstm, HCRYPTHASH hcrypthash, HCRYPTKEY hcryptkey)
	{
	BiffWriter bw(pstm, hcrypthash, hcryptkey);

#ifdef VBA
	bw.WriteInt(FID(PIID), ApcProjectItem.ID());
#endif
	bw.WriteStruct(FID(VCEN), &m_d.m_v, sizeof(Vertex));
    bw.WriteWideString(FID(COLC), (WCHAR *)m_d.m_wzCollection);
	bw.WriteFloat(FID(CTRX), m_d.m_vCenter.x);
	bw.WriteFloat(FID(CTRY), m_d.m_vCenter.y);
    bw.WriteInt(FID(UPTM), m_d.m_updateinterval);
	bw.WriteBool(FID(TMON), m_d.m_tdr.m_fTimerEnabled);
	bw.WriteInt(FID(TMIN), m_d.m_tdr.m_TimerInterval);

	bw.WriteWideString(FID(NAME), (WCHAR *)m_wzName);

	bw.WriteBool(FID(BGLS), m_fBackglass);

	bw.WriteTag(FID(ENDB));

	return S_OK;
	}

HRESULT LightSeq::InitLoad(IStream *pstm, PinTable *ptable, int *pid, int version, HCRYPTHASH hcrypthash, HCRYPTKEY hcryptkey)
	{
	SetDefaults();
#ifndef OLDLOAD
	BiffReader br(pstm, this, pid, version, hcrypthash, hcryptkey);

	m_ptable = ptable;

	br.Load();
	return S_OK;
#else
	ULONG read = 0;
	HRESULT hr = S_OK;

	m_ptable = ptable;

	DWORD dwID;
	if(FAILED(hr = pstm->Read(&dwID, sizeof dwID, &read)))
		return hr;

	if(FAILED(hr = pstm->Read(&m_d, sizeof(LightSeqData), &read)))
		return hr;

	*pid = dwID;

	return hr;
#endif
	}

BOOL LightSeq::LoadToken(int id, BiffReader *pbr)
	{
	if (id == FID(PIID))
		{
		pbr->GetInt((int *)pbr->m_pdata);
		}
	else if (id == FID(VCEN))
		{
		pbr->GetStruct(&m_d.m_v, sizeof(Vertex));
		}
	else if (id == FID(COLC))
		{
		pbr->GetWideString((WCHAR *)m_d.m_wzCollection);
		}
	else if (id == FID(CTRX))
		{
            pbr->GetFloat(&m_d.m_vCenter.x);
		}
	else if (id == FID(CTRY))
		{
            pbr->GetFloat(&m_d.m_vCenter.y);
		}
    else if (id == FID(UPTM))
		{
            pbr->GetInt(&m_d.m_updateinterval);
		}
	else if (id == FID(TMON))
		{
		pbr->GetBool(&m_d.m_tdr.m_fTimerEnabled);
		}
	else if (id == FID(TMIN))
		{
		pbr->GetInt(&m_d.m_tdr.m_TimerInterval);
		}
	else if (id == FID(NAME))
		{
		pbr->GetWideString((WCHAR *)m_wzName);
		}
	else if (id == FID(BGLS))
		{
		pbr->GetBool(&m_fBackglass);
		}
	return fTrue;
	}

HRESULT LightSeq::InitPostLoad()
{
	return S_OK;
}

/*int LightSeq::GetDialogID()
{
	return IDD_PROPLIGHTSEQ;
}*/

void LightSeq::GetDialogPanes(Vector<PropertyPane> *pvproppane)
	{
	PropertyPane *pproppane;

	pproppane = new PropertyPane(IDD_PROP_NAME, NULL);
	pvproppane->AddElement(pproppane);

	pproppane = new PropertyPane(IDD_PROPLIGHTSEQ_POSITION, IDS_POSITION);
	pvproppane->AddElement(pproppane);

	pproppane = new PropertyPane(IDD_PROPLIGHTSEQ_STATE, IDS_STATE);
	pvproppane->AddElement(pproppane);

	pproppane = new PropertyPane(IDD_PROP_TIMER, IDS_MISC);
	pvproppane->AddElement(pproppane);
	}


STDMETHODIMP LightSeq::get_Collection(BSTR *pVal)
{
	OLECHAR wz[512];

	memcpy (wz, m_d.m_wzCollection, sizeof(m_d.m_wzCollection));
	*pVal = SysAllocString(wz);

	return S_OK;
}

STDMETHODIMP LightSeq::put_Collection(BSTR newVal)
{
	STARTUNDO
	memcpy (m_d.m_wzCollection, (void *)newVal, sizeof(m_d.m_wzCollection));
	STOPUNDO

	return S_OK;
}

STDMETHODIMP LightSeq::get_CenterX(float *pVal)
{
	*pVal = m_d.m_vCenter.x;

	return S_OK;
}

STDMETHODIMP LightSeq::put_CenterX(float newVal)
{
	if ((newVal < 0) || (newVal >= 1000))
		return E_FAIL;
	STARTUNDO
	m_d.m_vCenter.x = newVal;
	// set the centre point of the grid for effects which start from the center
	m_GridXCenter = (float)((int)(m_d.m_vCenter.x / LIGHTSEQGRIDSCALE));
	m_GridXCenterAdjust	= abs( (int)m_lightSeqGridWidth/2 - (int)m_GridXCenter);
	STOPUNDO

	return S_OK;
}

STDMETHODIMP LightSeq::get_CenterY(float *pVal)
{
	*pVal = m_d.m_vCenter.y;

	return S_OK;
}

STDMETHODIMP LightSeq::put_CenterY(float newVal)
{
	if ((newVal < 0) || (newVal >= 2000))
		return E_FAIL;

	STARTUNDO
	m_d.m_vCenter.y = newVal;
	// set the centre point of the grid for effects which start from the center
	m_GridYCenter = (float)((int)(m_d.m_vCenter.y / LIGHTSEQGRIDSCALE));
	m_GridYCenterAdjust = abs( (int)m_lightSeqGridHeight/2 - (int)m_GridYCenter);
	STOPUNDO

	return S_OK;
}

STDMETHODIMP LightSeq::get_UpdateInterval(long *pVal)
{
    *pVal = m_d.m_updateinterval;

    return S_OK;
}

STDMETHODIMP LightSeq::put_UpdateInterval(long newVal)
{
	STARTUNDO
    m_d.m_updateinterval = max(1, newVal);
	STOPUNDO
	return S_OK;
}

STDMETHODIMP LightSeq::Play(SequencerState Animation, long TailLength, long Repeat, long Pause)
{
	HRESULT	rc;
	int		newHead;

	rc = S_OK;

	// sanity check the parameters
	if (TailLength < 0)
	{
		TailLength = 0;
	}
	if (Repeat <= 0)
	{
		Repeat = 1;
	}
	if (Pause < 0)
	{
		Pause = 0;
	}

	// 'all lights on' and 'all lights off' are directly processed and not put into the queue
	if (Animation == SeqAllOn)
	{
		// turn on all lights
		SetAllLightsToState(LightStateOn);
	}
	else
	{
		if (Animation == SeqAllOff)
		{
			// turn off all lights
			SetAllLightsToState(LightStateOff);
		}
		else
		{
	   	   	// move the head of the queue to the next position
	   	   	newHead = m_queue.Head + 1;
	   	   	// handle the wrap around (circlular queue)
	   	   	if (newHead >= LIGHTSEQQUEUESIZE)
	   	   	{
	   	   		newHead = 0;
	   	   	}
	   	   	// if the queue is full, then bomb out
	   	   	if (newHead == m_queue.Tail)
	   	   	{
	   	   		rc = E_FAIL;
	   	   	}
	   	   	else
	   	   	{
	   	   		// else load up the queue
	   	   		m_queue.Data[m_queue.Head].Animation = Animation;
	   	   		m_queue.Data[m_queue.Head].TailLength = TailLength;
	   	   		m_queue.Data[m_queue.Head].Repeat = Repeat;
	   	   		m_queue.Data[m_queue.Head].Pause = Pause;
				m_queue.Data[m_queue.Head].UpdateRate = m_d.m_updateinterval;
	   	   		m_queue.Head = newHead;
	   	   	}
		}
	}
	return rc;
}

STDMETHODIMP LightSeq::StopPlay(void)
{
	LightState		state;
	ItemTypeEnum	type;

	// no animation in progress
	m_playInProgress = false;
	m_pauseInProgress = false;
	// turn off any tracers
	m_th1.type = eSeqNull;
	m_th2.type = eSeqNull;
	m_tt1.type = eSeqNull;
	m_tt2.type = eSeqNull;
	// flush the queue
	m_queue.Head = 0;
	m_queue.Tail = 0;

	// Reset lights back to original state
	if (m_pcollection != NULL)
	{
		int i;
		int size = m_pcollection->m_visel.Size();
		for(i=0; i<size; i++)
		{
			type = m_pcollection->m_visel.ElementAt(i)->GetIEditable()->GetItemType();
			if (type == eItemLight)
			{
				Light *pLight = (Light *)m_pcollection->m_visel.ElementAt(i);
				pLight->unLockLight();
				pLight->get_State(&state);
				pLight->put_State(state);
			}
			else if (type == eItemBumper)
			{
				Bumper *pBumper = (Bumper *)m_pcollection->m_visel.ElementAt(i);
				pBumper->unLockLight();
				pBumper->get_State(&state);
				pBumper->put_State(state);
			}
		}
	}
	return S_OK;
}



void LightSeq::SetupTracers(SequencerState Animation, long TailLength, long Repeat, long Pause)
{
	bool	inverse;

	// no animation in progress
	m_playInProgress = false;
	m_pauseInProgress = false;

	// turn off any tracers
	m_th1.type = eSeqNull;
	m_th2.type = eSeqNull;
	m_tt1.type = eSeqNull;
	m_tt2.type = eSeqNull;

	// remember the current sequence (before it gets altered)
	m_playAnimation	= Animation;
	m_tailLength 	= TailLength;
	m_replayCount 	= Repeat;
	m_pauseValue 	= Pause;

	inverse = false;

	switch (Animation)
	{
		// blink all lights on and off
		case SeqBlinking:
			m_th1.type			= eSeqBlink;
			m_th1.length		= 0;
			m_th1.frameCount	= 2;
			break;

		// Randomly turn lights on and off for the pause period
		case SeqRandom:
			m_th1.type			= eSeqRandom;
			m_th1.length		= TailLength;
			m_th1.frameCount	= Pause / m_updateRate;
			// no repeat or pause for this effect and certainly no tail
			TailLength 	= 0;
			Repeat		= 1;
			Pause		= 0;
			m_tailLength  = TailLength;
			m_replayCount = Repeat;
			m_pauseValue  = Pause;
			break;

		// Turn on all lights starting at the bottom of the playfield and moving up
		case SeqUpOff:
			inverse				= true;
		case SeqUpOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 1;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.stepY			= -1;
			m_th1.processStepY 	= 0;
			m_th1.length		= m_lightSeqGridWidth;
			m_th1.frameCount	= m_lightSeqGridHeight;
			break;

		// Turn on all lights starting at the top of the playfield and moving down
		case SeqDownOff:
			inverse				= true;
		case SeqDownOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 1;
			m_th1.y				= 0;
			m_th1.stepY			= 1;
			m_th1.processStepY 	= 0;
			m_th1.length		= m_lightSeqGridWidth;
			m_th1.frameCount	= m_lightSeqGridHeight;
			break;

		// Turn on all lights starting at the left of the playfield and moving right
		case SeqRightOff:
			inverse				= true;
		case SeqRightOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 1;
			m_th1.processStepX 	= 0;
			m_th1.y				= 0;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= 1;
			m_th1.length		= m_lightSeqGridHeight;
			m_th1.frameCount	= m_lightSeqGridWidth;
			TailLength 			/= 2;
			break;

		// Turn on all lights starting at the right of the playfield and moving left
		case SeqLeftOff:
			inverse				= true;
		case SeqLeftOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.stepX			= -1;
			m_th1.processStepX 	= 0;
			m_th1.y				= 0;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= 1;
			m_th1.length		= m_lightSeqGridHeight;
			m_th1.frameCount	= m_lightSeqGridWidth;
			TailLength 			/= 2;
			break;

		// Turn on all lights starting at the bottom/left of the playfield and diagonally up
		case SeqDiagUpRightOff:
			inverse				= true;
		case SeqDiagUpRightOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 0.5f;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.stepY			= -1;
			m_th1.processStepY 	= 1;
			m_th1.length		= m_lightSeqGridWidth*2;
			m_th1.frameCount	= m_lightSeqGridHeight + (m_lightSeqGridWidth*2);
			break;

		// Turn on all lights starting at the bottom/right of the playfield and diagonally up
		case SeqDiagUpLeftOff:
			inverse				= true;
		case SeqDiagUpLeftOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= -0.5f;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.stepY			= -1;
			m_th1.processStepY 	= 1;
			m_th1.length		= m_lightSeqGridWidth*2;
			m_th1.frameCount	= m_lightSeqGridHeight + (m_lightSeqGridWidth*2);
			break;

		// Turn on all lights starting at the top/left of the playfield and diagonally down
		case SeqDiagDownRightOff:
			inverse				= true;
		case SeqDiagDownRightOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 0.5f;
			m_th1.y				= 0;
			m_th1.stepY			= 1;
			m_th1.processStepY 	= -1;
			m_th1.length		= m_lightSeqGridWidth*2;
			m_th1.frameCount	= m_lightSeqGridHeight + (m_lightSeqGridWidth*2);
			break;

		// Turn on all lights starting at the top/right of the playfield and diagonally down
		case SeqDiagDownLeftOff:
			inverse				= true;
		case SeqDiagDownLeftOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= -0.5f;
			m_th1.y				= 0;
			m_th1.stepY			= 1;
			m_th1.processStepY 	= -1;
			m_th1.length		= m_lightSeqGridWidth*2;
			m_th1.frameCount	= m_lightSeqGridHeight + (m_lightSeqGridWidth*2);
			break;

		// Turn on all lights starting in the middle and moving outwards to the side edges
		case SeqMiddleOutHorizOff:
			inverse				= true;
		case SeqMiddleOutHorizOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= m_GridXCenter-1;
			m_th1.stepX			= -1;
			m_th1.processStepX 	= 0;
			m_th1.y				= 0;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= 1;
			m_th1.length		= m_lightSeqGridHeight;
			m_th1.frameCount	= m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th2.type			= eSeqLine;
			m_th2.x				= m_GridXCenter;
			m_th2.stepX			= 1;
			m_th2.processStepX 	= 0;
			m_th2.y				= 0;
			m_th2.stepY			= 0;
			m_th2.processStepY 	= 1;
			m_th2.length		= m_lightSeqGridHeight;
			m_th2.frameCount	= m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			TailLength 			/= 4;
			break;

		// Turn on all lights starting on the side edges and moving into the middle
		case SeqMiddleInHorizOff:
			inverse				= true;
		case SeqMiddleInHorizOn:
			{
			float effectlength	= (float)(m_lightSeqGridWidth/2+m_GridXCenterAdjust);
			m_th1.type			= eSeqLine;
			m_th1.x				= m_GridXCenter - effectlength;
			m_th1.stepX			= 1;
			m_th1.processStepX 	= 0;
			m_th1.y				= 0;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= 1;
			m_th1.length		= m_lightSeqGridHeight;
			m_th1.frameCount	= (int)effectlength+1;
			m_th2.type			= eSeqLine;
			m_th2.x				= m_GridXCenter + effectlength;
			m_th2.stepX			= -1;
			m_th2.processStepX 	= 0;
			m_th2.y				= 0;
			m_th2.stepY			= 0;
			m_th2.processStepY 	= 1;
			m_th2.length		= m_lightSeqGridHeight;
			m_th2.frameCount	= (int)effectlength;
			TailLength 			/= 4;
			}
			break;

		// Turn on all lights starting in the middle and moving outwards to the top and bottom
		case SeqMiddleOutVertOff:
			inverse				= true;
		case SeqMiddleOutVertOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 1;
			m_th1.y				= m_GridYCenter-1;
			m_th1.stepY			= -1;
			m_th1.processStepY 	= 0;
			m_th1.length		= (int)m_lightSeqGridWidth;
			m_th1.frameCount	= (int)m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			m_th2.type			= eSeqLine;
			m_th2.x				= 0;
			m_th2.stepX			= 0;
			m_th2.processStepX 	= 1;
			m_th2.y				= m_GridYCenter;
			m_th2.stepY			= 1;
			m_th2.processStepY 	= 0;
			m_th2.length		= m_lightSeqGridWidth;
			m_th2.frameCount	= m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			TailLength 			/= 2;
			break;

		// Turn on all lights starting on the top and bottom edges and moving inwards to the middle
		case SeqMiddleInVertOff:
			inverse				= true;
		case SeqMiddleInVertOn:
			{
			float effectlength	= (float)(m_lightSeqGridHeight/2+m_GridYCenterAdjust);
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 1;
			m_th1.y				= m_GridYCenter - effectlength;
			m_th1.stepY			= 1;
			m_th1.processStepY 	= 0;
			m_th1.length		= m_lightSeqGridWidth;
			m_th1.frameCount	= (int)effectlength+1;
			m_th2.type			= eSeqLine;
			m_th2.x				= 0;
			m_th2.stepX			= 0;
			m_th2.processStepX 	= 1;
			m_th2.y				= m_GridYCenter + effectlength;
			m_th2.stepY			= -1;
			m_th2.processStepY 	= 0;
			m_th2.length		= m_lightSeqGridWidth;
			m_th2.frameCount	= (int)effectlength;
			TailLength 			/= 2;
			}
			break;

		// top half of the playfield wipes on to the right while the bottom half wipes on to the left
		case SeqStripe1HorizOff:
			inverse				= true;
		case SeqStripe1HorizOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 1;
			m_th1.processStepX 	= 0;
			m_th1.y				= m_GridYCenter-1;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= -1;
			m_th1.length		= m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			m_th1.frameCount	= m_lightSeqGridWidth;
			m_th2.type			= eSeqLine;
			m_th2.x				= (float)m_lightSeqGridWidth-1;
			m_th2.stepX			= -1;
			m_th2.processStepX 	= 0;
			m_th2.y				= m_GridYCenter;
			m_th2.stepY			= 0;
			m_th2.processStepY 	= 1;
			m_th2.length		= m_lightSeqGridHeight/2+m_GridXCenterAdjust;
			m_th2.frameCount	= m_lightSeqGridWidth;
			TailLength 			/= 2;
			break;

		// top half of the playfield wipes on to the left while the bottom half wipes on to the right
		case SeqStripe2HorizOff:
			inverse				= true;
		case SeqStripe2HorizOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.stepX			= -1;
			m_th1.processStepX 	= 0;
			m_th1.y				= m_GridYCenter-1;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= -1;
			m_th1.length		= m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			m_th1.frameCount	= m_lightSeqGridWidth;
			m_th2.type			= eSeqLine;
			m_th2.x				= 0;
			m_th2.stepX			= 1;
			m_th2.processStepX 	= 0;
			m_th2.y				= m_GridYCenter;
			m_th2.stepY			= 0;
			m_th2.processStepY 	= 1;
			m_th2.length		= m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			m_th2.frameCount	= m_lightSeqGridWidth;
			TailLength 			/= 2;
			break;

		// left side of the playfield wipes on going up while the right side wipes on doing down
		case SeqStripe1VertOff:
			inverse				= true;
		case SeqStripe1VertOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= m_GridXCenter-1;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= -1;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.stepY			= -1;
			m_th1.processStepY 	= 0;
			m_th1.length		= m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th1.frameCount	= m_lightSeqGridHeight;
			m_th2.type			= eSeqLine;
			m_th2.x				= m_GridXCenter;
			m_th2.stepX			= 0;
			m_th2.processStepX 	= 1;
			m_th2.y				= 0;
			m_th2.stepY			= 1;
			m_th2.processStepY 	= 0;
			m_th2.length		= m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th2.frameCount	= m_lightSeqGridHeight;
			break;

		// left side of the playfield wipes on going down while the right side wipes on doing up
		case SeqStripe2VertOff:
			inverse				= true;
		case SeqStripe2VertOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= m_GridXCenter-1;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= -1;
			m_th1.y				= 0;
			m_th1.stepY			= 1;
			m_th1.processStepY 	= 0;
			m_th1.length		= m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th1.frameCount	= m_lightSeqGridHeight;
			m_th2.type			= eSeqLine;
			m_th2.x				= m_GridXCenter;
			m_th2.stepX			= 0;
			m_th2.processStepX 	= 1;
			m_th2.y				= (float)m_lightSeqGridHeight-1;
			m_th2.stepY			= -1;
			m_th2.processStepY 	= 0;
			m_th2.length		= m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th2.frameCount	= m_lightSeqGridHeight;
			break;

		// turn lights on, cross-hatch with even lines going right and odd lines going left
		case SeqHatch1HorizOff:
			inverse				= true;
		case SeqHatch1HorizOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 1;
			m_th1.processStepX 	= 0;
			m_th1.y				= 0;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= 2;
			m_th1.length		= m_lightSeqGridHeight/2;
			m_th1.frameCount	= m_lightSeqGridWidth;
			m_th2.type			= eSeqLine;
			m_th2.x				= (float)m_lightSeqGridWidth-1;
			m_th2.stepX			= -1;
			m_th2.processStepX 	= 0;
			m_th2.y				= 1;
			m_th2.stepY			= 0;
			m_th2.processStepY 	= 2;
			m_th2.length		= m_lightSeqGridHeight/2;
			m_th2.frameCount	= m_lightSeqGridWidth;
			TailLength 			/= 2;
			break;

		// turn lights on, cross-hatch with even lines going left and odd lines going right
		case SeqHatch2HorizOff:
			inverse				= true;
		case SeqHatch2HorizOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.stepX			= -1;
			m_th1.processStepX 	= 0;
			m_th1.y				= 0;
			m_th1.stepY			= 0;
			m_th1.processStepY 	= 2;
			m_th1.length		= m_lightSeqGridHeight/2;
			m_th1.frameCount	= m_lightSeqGridWidth;
			m_th2.type			= eSeqLine;
			m_th2.x				= 0;
			m_th2.stepX			= 1;
			m_th2.processStepX 	= 0;
			m_th2.y				= 1;
			m_th2.stepY			= 0;
			m_th2.processStepY 	= 2;
			m_th2.length		= m_lightSeqGridHeight/2;
			m_th2.frameCount	= m_lightSeqGridWidth;
			TailLength 			/= 2;
			break;

		// turn lights on, cross-hatch with even lines going up and odd lines going down
		case SeqHatch1VertOff:
			inverse				= true;
		case SeqHatch1VertOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 2;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.stepY			= -1;
			m_th1.processStepY 	= 0;
			m_th1.length		= m_lightSeqGridWidth/2;
			m_th1.frameCount	= m_lightSeqGridHeight;
			m_th2.type			= eSeqLine;
			m_th2.x				= 1;
			m_th2.stepX			= 0;
			m_th2.processStepX 	= 2;
			m_th2.y				= 0;
			m_th2.stepY			= 1;
			m_th2.processStepY 	= 0;
			m_th2.length		= m_lightSeqGridWidth/2;
			m_th2.frameCount	= m_lightSeqGridHeight;
			break;

		// turn lights on, cross-hatch with even lines going down and odd lines going up
		case SeqHatch2VertOff:
			inverse				= true;
		case SeqHatch2VertOn:
			m_th1.type			= eSeqLine;
			m_th1.x				= 0;
			m_th1.stepX			= 0;
			m_th1.processStepX 	= 2;
			m_th1.y				= 0;
			m_th1.stepY			= 1;
			m_th1.processStepY 	= 0;
			m_th1.length		= m_lightSeqGridWidth/2;
			m_th1.frameCount	= m_lightSeqGridHeight;
			m_th2.type			= eSeqLine;
			m_th2.x				= 1;
			m_th2.stepX			= 0;
			m_th2.processStepX 	= 2;
			m_th2.y				= (float)m_lightSeqGridHeight-1;
			m_th2.stepY			= -1;
			m_th2.processStepY 	= 0;
			m_th2.length		= m_lightSeqGridWidth/2;
			m_th2.frameCount	= m_lightSeqGridHeight;
			break;

		// turn on all the lights, starting in the table center and circle out
		case SeqCircleOutOff:
			inverse				= true;
		case SeqCircleOutOn:
			m_th1.type			= eSeqCircle;
			m_th1.radius		= 0;
			m_th1.stepRadius	= 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= m_GridYCenter;
			m_th1.frameCount	= m_lightSeqGridWidth/2+m_GridXCenterAdjust+m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			break;

		// turn on all the lights, starting at the table edges and circle in
		case SeqCircleInOff:
			inverse				= true;
		case SeqCircleInOn:
			m_th1.type			= eSeqCircle;
			m_th1.radius		= (float)(m_lightSeqGridWidth/2+m_GridXCenterAdjust+m_lightSeqGridHeight/2+m_GridYCenterAdjust);
			m_th1.stepRadius	= -1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= m_GridYCenter;
			m_th1.frameCount	= m_lightSeqGridWidth/2+m_GridXCenterAdjust+m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			break;

		// turn all the lights on starting in the middle and sweeping around to the right
		case SeqClockRightOff:
			inverse				= true;
		case SeqClockRightOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= m_GridYCenter;
			m_th1.length		= m_lightSeqGridWidth/2+m_GridXCenterAdjust+m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			m_th1.frameCount	= 360;
			break;

		// turn all the lights on starting in the middle and sweeping around to the left
		case SeqClockLeftOff:
			inverse				= true;
		case SeqClockLeftOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= m_GridYCenter;
			m_th1.length		= m_lightSeqGridWidth/2+m_GridXCenterAdjust+m_lightSeqGridHeight/2+m_GridYCenterAdjust;
			m_th1.frameCount	= 360;
			break;

		// turn all the lights on starting in the middle/bottom and sweeping around to the right
		case SeqRadarRightOff:
			inverse				= true;
		case SeqRadarRightOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 270;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the middle/bottom and sweeping around to the right
		case SeqRadarLeftOff:
			inverse				= true;
		case SeqRadarLeftOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 90;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the middle/yop and sweeping around to the right
		case SeqWiperRightOff:
			inverse				= true;
		case SeqWiperRightOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 270;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= 0;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the middle/top and sweeping around to the right
		case SeqWiperLeftOff:
			inverse				= true;
		case SeqWiperLeftOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 90;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= 0;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the middle/left edge and sweeping up
		case SeqFanLeftUpOff:
			inverse				= true;
		case SeqFanLeftUpOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 180;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = 1;
			m_th1.x				= 0;
			m_th1.y				= m_GridYCenter;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the middle/left edge and sweeping down
		case SeqFanLeftDownOff:
			inverse				= true;
		case SeqFanLeftDownOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = 1;
			m_th1.x				= 0;
			m_th1.y				= m_GridYCenter;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the middle/right edge and sweeping up
		case SeqFanRightUpOff:
			inverse				= true;
		case SeqFanRightUpOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 180;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = -1;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.y				= m_GridYCenter;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the middle/right edge and sweeping down
		case SeqFanRightDownOff:
			inverse				= true;
		case SeqFanRightDownOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = -1;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.y				= m_GridYCenter;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth+m_GridYCenterAdjust+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			break;

		// turn all the lights on starting in the bottom/left corner and arcing up
		case SeqArcBottomLeftUpOff:
			inverse				= true;
		case SeqArcBottomLeftUpOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 90;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = 1;
			m_th1.x				= 0;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the bottom/left corner and arcing down
		case SeqArcBottomLeftDownOff:
			inverse				= true;
		case SeqArcBottomLeftDownOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = 1;
			m_th1.x				= 0;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the bottom/right corner and arcing up
		case SeqArcBottomRightUpOff:
			inverse				= true;
		case SeqArcBottomRightUpOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 90;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = -1;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the bottom/right corner and arcing down
		case SeqArcBottomRightDownOff:
			inverse				= true;
		case SeqArcBottomRightDownOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 180;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = -1;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.y				= (float)m_lightSeqGridHeight-1;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the top/left corner and arcing up
		case SeqArcTopLeftUpOff:
			inverse				= true;
		case SeqArcTopLeftUpOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 180;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = 1;
			m_th1.x				= 0;
			m_th1.y				= 0;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the top/left corner and arcing down
		case SeqArcTopLeftDownOff:
			inverse				= true;
		case SeqArcTopLeftDownOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 90;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = 1;
			m_th1.x				= 0;
			m_th1.y				= 0;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the top/right corner and arcing up
		case SeqArcTopRightUpOff:
			inverse				= true;
		case SeqArcTopRightUpOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = -1;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.y				= 0;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the top/right corner and arcing down
		case SeqArcTopRightDownOff:
			inverse				= true;
		case SeqArcTopRightDownOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 90;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = -1;
			m_th1.x				= (float)m_lightSeqGridWidth-1;
			m_th1.y				= 0;
			m_th1.length		= m_lightSeqGridHeight+m_lightSeqGridWidth;
			m_th1.frameCount	= 90;
			break;

		// turn all the lights on starting in the centre and screwing (2 tracers) clockwise
		case SeqScrewRightOff:
			inverse				= true;
		case SeqScrewRightOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= 1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= m_GridYCenter-1;
			m_th1.length		= m_lightSeqGridHeight/2+m_GridYCenterAdjust+m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			m_th2.type			= eSeqRadar;
			m_th2.angle			= 180-1;
			m_th2.stepAngle		= 1;
			m_th2.processRadius = 1;
			m_th2.x				= m_GridXCenter;
			m_th2.y				= m_GridYCenter;
			m_th2.length		= m_lightSeqGridHeight/2+m_GridYCenterAdjust+m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th2.frameCount	= 180;
			break;

		// turn all the lights on starting in the centre and screwing (2 tracers) anti-clockwise
		case SeqScrewLeftOff:
			inverse				= true;
		case SeqScrewLeftOn:
			m_th1.type			= eSeqRadar;
			m_th1.angle			= 0;
			m_th1.stepAngle		= -1;
			m_th1.processRadius = 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= m_GridYCenter-1;
			m_th1.length		= m_lightSeqGridHeight/2+m_GridYCenterAdjust+m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th1.frameCount	= 180;
			m_th2.type			= eSeqRadar;
			m_th2.angle			= 180-1;
			m_th2.stepAngle		= -1;
			m_th2.processRadius = 1;
			m_th2.x				= m_GridXCenter;
			m_th2.y				= m_GridYCenter;
			m_th2.length		= m_lightSeqGridHeight/2+m_GridYCenterAdjust+m_lightSeqGridWidth/2+m_GridXCenterAdjust;
			m_th2.frameCount	= 180;
			break;

		// unknown/supported animation (should happen as the animation is a enum, but hey!)
		default:
			// cause it to expire in 1 frame and move onto the next animation
			m_th1.length		= 0;
			m_th1.frameCount	= 1;
			TailLength 			= 0;
			Repeat 				= 1;
			Pause 				= 0;
			break;
	}

	// no delays on the effects
	m_th1.delay	= 0;
	m_th2.delay	= 0;
	m_tt1.delay	= 0;
	m_tt2.delay	= 0;

	// is there a tail for this effect?
	if (TailLength != 0)
	{
		// then the head effect becomes the tail with the tail length used as the delay
		memcpy(&m_tt1, &m_th1, sizeof(_tracer));
		m_tt1.delay = TailLength;
		memcpy(&m_tt2, &m_th2, sizeof(_tracer));
		m_tt2.delay = TailLength;
	}

	// are we are in inverse mode (tail is the lead) or not?
	if (inverse == true)
	{
		_tracer	temp;
		// swap the head and tail over
		memcpy(&temp,  &m_tt1, sizeof(_tracer));
		memcpy(&m_tt1, &m_th1, sizeof(_tracer));
		memcpy(&m_th1, &temp,  sizeof(_tracer));

		memcpy(&temp,  &m_tt2, sizeof(_tracer));
		memcpy(&m_tt2, &m_th2, sizeof(_tracer));
		memcpy(&m_th2, &temp,  sizeof(_tracer));
	}

	m_timeNextUpdate = g_pplayer->m_timeCur + m_updateRate;
	m_playInProgress = true;
}

bool LightSeq::ProcessTracer(_tracer *pTracer, LightState State)
{
	bool 		rc;
	int 		i;
	float		fi;
	float 		x, y;
	int			size;
	int			randomLight;
	LightState	state;
	float		angle, angle2, angle3, angle4;
	float		sn, cs, sn2, cs2, sn3, cs3, sn4, cs4;

	rc = false;

	// if this tracer isn't valid, then exit with a finished return code
	if (pTracer->type == eSeqNull)
		return (true);

	if (pTracer->delay == 0)
	{
		switch(pTracer->type)
		{
	   		// process the blink type of effect
	   		case eSeqBlink:
	   			switch (pTracer->frameCount--)
	   			{
	   				case 2:
	   					SetAllLightsToState(LightStateOn);
	   		    	    m_timeNextUpdate = g_pplayer->m_timeCur + m_pauseValue;
	   					break;

	   				case 1:
	   					SetAllLightsToState(LightStateOff);
	   		    	    m_timeNextUpdate = g_pplayer->m_timeCur + m_pauseValue;
	   					break;

	   				case 0:
	   					rc = true;
	   					break;
	   			}
	   			break;

	   		// process the random type of effect
	   		case eSeqRandom:
				// get the number of elements in this
				size = m_pcollection->m_visel.Size();
				// randomly pick n elements and invert their state
				for (i=0; i<pTracer->length; i++)
				{
					// Generates integer random number 0..(size-1)
					randomLight = (int)(size * ( ((float)rand()) / ((float)(RAND_MAX+1)) ) );
					// get the state of this light
					state = GetElementState(randomLight);
					// invert the state
					if (state == LightStateOn)
					{
						state = LightStateOff;
					}
					else
					{
						state = LightStateOn;
					}
					SetElementToState(randomLight, state);
				}

				pTracer->frameCount -= 1;
	   			if (pTracer->frameCount == 0)
	   			{
	   				// nullify this tracer
	   				pTracer->type = eSeqNull;
	   				rc = true;
	   			}
				break;

	   		// process the line tracers
	   		case eSeqLine:
	   			// get the start of the trace line
	   			x = pTracer->x;
	   			y = pTracer->y;

	   			for (i=0; i<pTracer->length; i++)
	   			{
	   				VerifyAndSetGridElement((int)x, (int)y, State);
	   				// move to the next position in the line
	   				x += pTracer->processStepX;
	   				y += pTracer->processStepY;
	   			}

	   			pTracer->frameCount -= 1;
	   			if (pTracer->frameCount == 0)
	   			{
	   				// nullify this tracer
	   				pTracer->type = eSeqNull;
	   				rc = true;
	   			}
				else
				{
		   			// move to the next grid position
		   			pTracer->x += pTracer->stepX;
	   				pTracer->y += pTracer->stepY;
				}
	   			break;

	   		// process the circle type of effect
	   		case eSeqCircle:
				for (fi=0;fi<360;fi+=0.5f)
				{
					angle = ((PI*2)/360)*fi;
					sn = (float)sin(angle);
					cs = (float)cos(angle);
					x = pTracer->x + sn*pTracer->radius;
					y = pTracer->y - cs*pTracer->radius;
	   				VerifyAndSetGridElement((int)x, (int)y, State);
				}
	   			pTracer->frameCount -= 1;
	   			if (pTracer->frameCount == 0)
	   			{
	   				// nullify this tracer
	   				pTracer->type = eSeqNull;
	   				rc = true;
	   			}
				else
				{
					pTracer->radius += pTracer->stepRadius;
				}
				break;

			// process the radar type of effect
			case eSeqRadar:
				angle = pTracer->angle / 360 * PI * 2;
				sn = (float)sin(angle);
				cs = (float)cos(angle);
				// we need to process an extra 3 quarter steps for radar effects or it tends to miss lights
				// when the radius get big and setting the step to .250 makes the effect too slow
				angle2 = (pTracer->angle + pTracer->stepAngle/4) / 360 * PI * 2;
				sn2 = (float)sin(angle2);
				cs2 = (float)cos(angle2);
				angle3 = (pTracer->angle + ((pTracer->stepAngle/4)*2)) / 360 * PI * 2;
				sn3 = (float)sin(angle3);
				cs3 = (float)cos(angle3);
				angle4 = (pTracer->angle + ((pTracer->stepAngle/4)*3)) / 360 * PI * 2;
				sn4 = (float)sin(angle4);
				cs4 = (float)cos(angle4);

				if (m_th1.processRadius == 1)
				{
					for (i=0; i<pTracer->length; i++)
		   			{
						x = pTracer->x + sn*i;
						y = pTracer->y - cs*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
						x = pTracer->x + sn2*i;
						y = pTracer->y - cs2*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
						x = pTracer->x + sn3*i;
						y = pTracer->y - cs3*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
						x = pTracer->x + sn4*i;
						y = pTracer->y - cs4*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
					}
				}
				else
				{
					for (i=0; i<pTracer->length; i++)
		   			{
						x = pTracer->x - sn*i;
						y = pTracer->y + cs*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
						x = pTracer->x - sn2*i;
						y = pTracer->y + cs2*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
						x = pTracer->x - sn3*i;
						y = pTracer->y + cs3*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
						x = pTracer->x - sn4*i;
						y = pTracer->y + cs4*i;
	   					VerifyAndSetGridElement((int)x, (int)y, State);
					}
				}
	   			pTracer->frameCount -= 1;
	   			if (pTracer->frameCount == 0)
	   			{
	   				// nullify this tracer
	   				pTracer->type = eSeqNull;
	   				rc = true;
	   			}
				else
				{
					// move to the next angle
					pTracer->angle += pTracer->stepAngle;
					// process any wrap around
					if (pTracer->angle >= 360)
					{
						pTracer->angle -= 360;
					}
					if (pTracer->angle < 0)
					{
						pTracer->angle += 360;
					}
				}
				break;

	   		// invalid tracer type (shouldn't happen but hey!)
	   		default:
	   			rc = true;
	   	}
	}
	else
	{
		pTracer->delay -= 1;
	}
	return (rc);
}

void LightSeq::SetAllLightsToState(LightState State)
{
	int	i;
	int	size;

	if (m_pcollection != NULL)
	{
		size = m_pcollection->m_visel.Size();
		for(i=0; i<size; i++)
		{
			SetElementToState(i, State);
		}
	}
}

void LightSeq::SetElementToState(int index, LightState State)
{
	ItemTypeEnum	type;

	type = m_pcollection->m_visel.ElementAt(index)->GetIEditable()->GetItemType();
	if (type == eItemLight)
	{
		Light *pLight = (Light *)m_pcollection->m_visel.ElementAt(index);
		pLight->setLightStateBypass(State);
	}
	else if (type == eItemBumper)
	{
		Bumper *pBumper = (Bumper *)m_pcollection->m_visel.ElementAt(index);
		pBumper->setLightStateBypass(State);
	}
}

bool LightSeq::VerifyAndSetGridElement(int x, int y, LightState State)
{

	if ( ((x >=0) && (x < m_lightSeqGridWidth)) &&
		 ((y >=0) && (y < m_lightSeqGridHeight)) )
	{
		int	register gridIndex = (y * m_lightSeqGridWidth) + x;

		int	register index = m_pgridData[gridIndex];
		if (index != 0)
		{
			index--;
 			SetElementToState(index, State);
		}
		return (true);
	}
	else
	{
		return (false);
	}
}

LightState LightSeq::GetElementState(int index)
{
	ItemTypeEnum	type;
	LightState		rc;

	// just incase the element isn't a light or bumper
	rc = LightStateOff;

	type = m_pcollection->m_visel.ElementAt(index)->GetIEditable()->GetItemType();
	if (type == eItemLight)
	{
		Light *pLight = (Light *)m_pcollection->m_visel.ElementAt(index);
		rc = pLight->m_realState;
	}
	else if (type == eItemBumper)
	{
		Bumper *pBumper = (Bumper *)m_pcollection->m_visel.ElementAt(index);
		rc = pBumper->m_realState;
	}
	return (rc);
}


// Methods used by the light sequencer center marker

LightSeqCenter::LightSeqCenter(LightSeq *pLightSeq)
{
	m_pLightSeq = pLightSeq;
}

HRESULT LightSeqCenter::GetTypeName(BSTR *pVal)
{
return m_pLightSeq->GetTypeName(pVal);
}

IDispatch *LightSeqCenter::GetDispatch()
{
	return m_pLightSeq->GetDispatch();
}

/*int LightSeqCenter::GetDialogID()
{
	return m_pLightSeq->GetDialogID();
}*/

void LightSeqCenter::GetDialogPanes(Vector<PropertyPane> *pvproppane)
	{
	m_pLightSeq->GetDialogPanes(pvproppane);
	}

void LightSeqCenter::Delete()
{
	m_pLightSeq->Delete();
}

void LightSeqCenter::Uncreate()
{
	m_pLightSeq->Uncreate();
}

IEditable *LightSeqCenter::GetIEditable()
{
	return (IEditable *)m_pLightSeq;
}

PinTable *LightSeqCenter::GetPTable()
{
	return m_pLightSeq->GetPTable();
}

void LightSeqCenter::GetCenter(Vertex *pv)
{
	*pv = m_pLightSeq->m_d.m_vCenter;
}

void LightSeqCenter::PutCenter(Vertex *pv)
{
	m_pLightSeq->m_d.m_vCenter = *pv;
}

void LightSeqCenter::MoveOffset(float dx, float dy)
{
	m_pLightSeq->m_d.m_vCenter.x += dx;
	m_pLightSeq->m_d.m_vCenter.y += dy;

	GetPTable()->SetDirtyDraw();
}

int LightSeqCenter::GetSelectLevel()
{
	return 2;
}





// un-used code but kept

#if 0
		// turn on all lights starting in the centre and scrolling (screwing) clockwise out
		case SeqTwirlOutRightOff:
			inverse				= true;
		case SeqTwirlOutRightOn:
			m_th1.type			= eSeqTwirl;
			m_th1.radius		= 0;
			m_th1.stepRadius	= 1;
			m_th1.angle			= 0;
			m_th1.stepAngle		= 1;
			m_th1.x				= m_GridXCenter;
			m_th1.y				= m_GridYCenter;
			m_th1.length		= 30;
			m_th1.frameCount	= (360/m_th1.length)*(m_lightSeqGridHeight/2+m_GridYCenterAdjust);
			break;

		// turn on all lights starting in the centre and scrolling (screwing) anti-clockwise out
		case SeqTwirlOutLeftOff:
			inverse				= true;
		case SeqTwirlOutLeftOn:
			break;

		// turn on all lights starting in the outside and scrolling (screwing) clockwise in
		case SeqTwirlInRightOff:
			inverse				= true;
		case SeqTwirlInRightOn:
			break;

		// turn on all lights starting in the outside and scrolling (screwing) anti-clockwise in
		case SeqTwirlInLeftOff:
			inverse				= true;
		case SeqTwirlInLeftOn:
			break;


			// process the twirl type of effect
			case eSeqTwirl:
				for (i=0; i<pTracer->length; i++)
	   			{
					angle = ((PI*2)/360)*pTracer->angle;
					sn = (float)sin(angle);
					cs = (float)cos(angle);
					x = pTracer->x + sn*pTracer->radius;
					y = pTracer->y - cs*pTracer->radius;
	   				VerifyAndSetGridElement((int)x, (int)y, State);

					// move to the next angle
					pTracer->angle += pTracer->stepAngle;
					// process any wrap around
					if (pTracer->angle >= 360)
					{
						pTracer->angle -= 360;
						pTracer->radius += pTracer->stepRadius;
					}
					if (pTracer->angle < 0)
					{
						pTracer->angle += 360;
						pTracer->radius += pTracer->stepRadius;
					}
				}

	   			pTracer->frameCount -= 1;
	   			if (pTracer->frameCount == 0)
	   			{
	   				// nullify this tracer
	   				pTracer->type = eSeqNull;
	   				rc = true;
	   			}
				break;
#endif
