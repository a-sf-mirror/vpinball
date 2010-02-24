// Bumper.h: Definition of the Bumper class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUMPER_H__9A202FF0_7FAE_49BF_AA4C_C01C692E6DD9__INCLUDED_)
#define AFX_BUMPER_H__9A202FF0_7FAE_49BF_AA4C_C01C692E6DD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols

class IBlink
	{
public:
	virtual void DrawFrame(BOOL fOn) = 0;

	char m_rgblinkpattern[33];
	int m_blinkinterval;

	int m_timenextblink;
	int m_iblinkframe;
	};

class BumperData
	{
public:
	Vertex m_vCenter;
	float m_radius;
	float m_threshold; // speed at which ball needs to hit to register a hit
	float m_force; // force the bumper kicks back with
	TimerDataRoot m_tdr;
	float m_overhang;
	COLORREF m_color;
	COLORREF m_sidecolor;
	char m_szImage[MAXTOKEN];
	char m_szSurface[MAXTOKEN];
	LightState m_state;
	BOOL m_fFlashWhenHit; // Hacky flag for cool auto-behavior
	BOOL m_fCastsShadow;

	//char m_rgblinkpattern[33];
	//int m_blinkinterval;
	};

/////////////////////////////////////////////////////////////////////////////
// Bumper

class Bumper :
	//public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IBumper, &IID_IBumper, &LIBID_VBATESTLib>,
	//public ISupportErrorInfo,
	public CComObjectRoot,
	public CComCoClass<Bumper,&CLSID_Bumper>,
#ifdef VBA
	public CApcProjectItem<Bumper>,
#endif
	public EventProxy<Bumper, &DIID_IBumperEvents>,
	public IConnectionPointContainerImpl<Bumper>,
	public IProvideClassInfo2Impl<&CLSID_Bumper, &DIID_IBumperEvents, &LIBID_VBATESTLib>,
	public ISelect,
	public IEditable,
	public Hitable,
	public IScriptable,
	public IBlink,
	public IFireEvents,
	public IPerPropertyBrowsing // Ability to fill in dropdown in property browser
	//public EditableImpl<Bumper>
{
public:
	Bumper();
	~Bumper();

BEGIN_COM_MAP(Bumper)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IBumper)
	//COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(Bumper)
// Remove the comment from the line above if you don't want your object to
// support aggregation.

STANDARD_DISPATCH_DECLARE
STANDARD_EDITABLE_DECLARES(eItemBumper)

BEGIN_CONNECTION_POINT_MAP(Bumper)
	CONNECTION_POINT_ENTRY(DIID_IBumperEvents)
END_CONNECTION_POINT_MAP()

DECLARE_REGISTRY_RESOURCEID(IDR_Bumper)
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	virtual void MoveOffset(float dx, float dy);
	virtual void SetObjectPos();
	virtual void RenderShadow(ShadowSur *psur, float height);

	//virtual HRESULT GetTypeName(BSTR *pVal);
	//virtual int GetDialogID();
	virtual void GetDialogPanes(Vector<PropertyPane> *pvproppane);

	// Multi-object manipulation
	virtual void GetCenter(Vertex *pv);
	virtual void PutCenter(Vertex *pv);

	virtual void DrawFrame(BOOL fOn);

	PinTable *m_ptable;

	BumperData m_d;

	BumperHitCircle *m_pbumperhitcircle;
	BOOL m_fOn; // Whether the light is actually currently on (different than the state, because of blinking)

	//int m_iblinkframe;
	//int m_timenextblink;

//>>> Added By Chris
	BOOL		m_fDisabled;
	BOOL		m_fLockedByLS;
	LightState 	m_realState;
	void		lockLight();
	void		unLockLight();
	void		setLightStateBypass(LightState newVal);
	void		setLightState(LightState newVal);
//<<<

// IBumper
public:
	STDMETHOD(get_SideColor)(/*[out, retval]*/ OLE_COLOR *pVal);
	STDMETHOD(put_SideColor)(/*[in]*/ OLE_COLOR newVal);
	STDMETHOD(get_FlashWhenHit)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_FlashWhenHit)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_BlinkInterval)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_BlinkInterval)(/*[in]*/ long newVal);
	STDMETHOD(get_BlinkPattern)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_BlinkPattern)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_State)(/*[out, retval]*/ LightState *pVal);
	STDMETHOD(put_State)(/*[in]*/ LightState newVal);
	STDMETHOD(get_Surface)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Surface)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Y)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_Y)(/*[in]*/ float newVal);
	STDMETHOD(get_X)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_X)(/*[in]*/ float newVal);
	STDMETHOD(get_Image)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Image)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Color)(/*[out, retval]*/ OLE_COLOR *pVal);
	STDMETHOD(put_Color)(/*[in]*/ OLE_COLOR newVal);
	STDMETHOD(get_Overhang)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_Overhang)(/*[in]*/ float newVal);
	STDMETHOD(get_Threshold)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_Threshold)(/*[in]*/ float newVal);
	STDMETHOD(get_Force)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_Force)(/*[in]*/ float newVal);
	STDMETHOD(get_Radius)(/*[out, retval]*/ float *pVal);
	STDMETHOD(put_Radius)(/*[in]*/ float newVal);
	STDMETHOD(get_CastsShadow)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_CastsShadow)(/*[in]*/ VARIANT_BOOL newVal);
	STDMETHOD(get_Disabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
	STDMETHOD(put_Disabled)(/*[in]*/ VARIANT_BOOL newVal);
};

/*class PIEventHandler : public CComObjectRootEx<CComSingleThreadModel>,
	public IApcProjectItemEvents
	{
BEGIN_COM_MAP(PIEventHandler)
END_COM_MAP()
	virtual HRESULT STDMETHODCALLTYPE View();
	virtual HRESULT STDMETHODCALLTYPE Activate();
	virtual HRESULT STDMETHODCALLTYPE CreateInstance(IDispatch __RPC_FAR *__RPC_FAR *Instance);
	virtual HRESULT STDMETHODCALLTYPE ReleaseInstances();
	virtual HRESULT STDMETHODCALLTYPE InstanceCreated(IDispatch __RPC_FAR *Instance);
	long m_advisecookie;
	};*/

#endif // !defined(AFX_BUMPER_H__9A202FF0_7FAE_49BF_AA4C_C01C692E6DD9__INCLUDED_)

