// 3dprimitive.h: interface for the 3dprimitive class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(AFX_HITTARGET_H__A67DE998_7D97_4E03_BE91_55BFD3A48DB6__INCLUDED_)
#define AFX_HITTARGET_H__A67DE998_7D97_4E03_BE91_55BFD3A48DB6__INCLUDED_

#include "resource.h"
#include <set>

// Indices for RotAndTra:
//     RotX = 0
//     RotY = 1
//     RotZ = 2
//     TraX = 3
//     TraY = 4
//     TraZ = 5
//  ObjRotX = 6
//  ObjRotY = 7
//  ObjRotZ = 8

class HitTargetData
{
public:
   Vertex3Ds m_vPosition;
   Vertex3Ds m_vSize;
   float m_rotZ;
   char m_szImage[MAXTOKEN];
   TargetType m_targetType;
   char m_szMaterial[32];

   TimerDataRoot m_tdr;

   float m_threshold;			// speed at which ball needs to hit to register a hit
   float m_elasticity;
   float m_elasticityFalloff;
   float m_friction;
   float m_scatter;
   float m_dropSpeed;
   bool  m_isDropped;
   U32   m_time_msec;

   float m_depthBias;      // for determining depth sorting
   bool m_fVisible;
   bool m_fDisableLighting;

   bool m_fUseHitEvent;
   bool m_fCollidable;
   bool m_fSkipRendering;
   bool m_fReflectionEnabled;
};

class HitTarget :
   public CComObjectRootEx<CComSingleThreadModel>,
   public IDispatchImpl<IHitTarget, &IID_IHitTarget, &LIBID_VPinballLib>,
   //public CComObjectRoot,
   public CComCoClass<HitTarget, &CLSID_HitTarget>,
   public EventProxy<HitTarget, &DIID_IHitTargetEvents>,
   public IConnectionPointContainerImpl<HitTarget>,
   public IProvideClassInfo2Impl<&CLSID_HitTarget, &DIID_IHitTargetEvents, &LIBID_VPinballLib>,

   public ISelect,
   public IEditable,
   public Hitable,
   public IScriptable,
   public IFireEvents,
   public IPerPropertyBrowsing // Ability to fill in dropdown in property browser
{
public:
   static const float DROP_TARGET_LIMIT;
    
   STDMETHOD(get_Material)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Material)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_Visible)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Visible)(/*[in]*/ VARIANT_BOOL newVal);

   STDMETHOD(get_X)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_X)(/*[in]*/ float newVal);
   STDMETHOD(get_Y)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_Y)(/*[in]*/ float newVal);
   STDMETHOD(get_Z)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_Z)(/*[in]*/ float newVal);
   STDMETHOD(get_ScaleX)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_ScaleX)(/*[in]*/ float newVal);
   STDMETHOD(get_ScaleY)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_ScaleY)(/*[in]*/ float newVal);
   STDMETHOD(get_ScaleZ)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_ScaleZ)(/*[in]*/ float newVal);

   STDMETHOD(get_Orientation)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_Orientation)(/*[in]*/ float newVal);

   STDMETHOD(get_Image)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_Image)(/*[in]*/ BSTR newVal);

   STDMETHOD(get_HasHitEvent)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_HasHitEvent)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_Threshold)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_Threshold)(/*[in]*/ float newVal);
   STDMETHOD(get_Collidable)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_Collidable)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_Elasticity)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_Elasticity)(/*[in]*/ float newVal);
   STDMETHOD(get_ElasticityFalloff)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_ElasticityFalloff)(/*[in]*/ float newVal);
   STDMETHOD(get_Friction)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_Friction)(/*[in]*/ float newVal);
   STDMETHOD(get_Scatter)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_Scatter)(/*[in]*/ float newVal);
   STDMETHOD(get_DepthBias)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_DepthBias)(/*[in]*/ float newVal);
   STDMETHOD(get_DropSpeed)(/*[out, retval]*/ float *pVal);
   STDMETHOD(put_DropSpeed)(/*[in]*/ float newVal);
   STDMETHOD(get_DisableLighting)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_DisableLighting)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_ReflectionEnabled)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_ReflectionEnabled)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_IsDropped)(/*[out, retval]*/ VARIANT_BOOL *pVal);
   STDMETHOD(put_IsDropped)(/*[in]*/ VARIANT_BOOL newVal);
   STDMETHOD(get_DrawStyle)(/*[out, retval]*/ TargetType *pVal);
   STDMETHOD(put_DrawStyle)(/*[in]*/ TargetType newVal);

   HitTarget();
   virtual ~HitTarget();

   BEGIN_COM_MAP(HitTarget)
      COM_INTERFACE_ENTRY(IDispatch)
      COM_INTERFACE_ENTRY(IHitTarget)

      COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)

      COM_INTERFACE_ENTRY(IPerPropertyBrowsing)

      COM_INTERFACE_ENTRY(IProvideClassInfo)
      COM_INTERFACE_ENTRY(IProvideClassInfo2)
   END_COM_MAP()

   BEGIN_CONNECTION_POINT_MAP(HitTarget)
      CONNECTION_POINT_ENTRY(DIID_IHitTargetEvents)
   END_CONNECTION_POINT_MAP()


   STANDARD_EDITABLE_DECLARES(HitTarget, eItemHitTarget, TARGET, 1)

      DECLARE_REGISTRY_RESOURCEID(IDR_HITTARGET)

   virtual void MoveOffset(const float dx, const float dy);
   virtual void SetObjectPos();
   // Multi-object manipulation
   virtual void GetCenter(Vertex2D * const pv) const;
   virtual void PutCenter(const Vertex2D * const pv);

   //STDMETHOD(get_Name)(BSTR *pVal) {return E_FAIL;}

   //virtual HRESULT InitVBA(BOOL fNew, int id, WCHAR *wzName);
   void WriteRegDefaults();
   virtual void GetDialogPanes(Vector<PropertyPane> *pvproppane);

   PinTable *m_ptable;

   virtual bool IsTransparent();
   virtual float GetDepth(const Vertex3Ds& viewDir);
   virtual unsigned long long GetMaterialID() { return m_ptable->GetMaterial(m_d.m_szMaterial)->hash(); }

   virtual void UpdatePropertyPanes();
   virtual void SetDefaultPhysics(bool fromMouseClick);

   void GenerateMesh(Vertex3D_NoTex2 *buf);
   void    TransformVertices();
   void    SetMeshType(const TargetType type);

   static INT_PTR CALLBACK ObjImportProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


   HitTargetData m_d;
   const Vertex3D_NoTex2 *m_vertices;
   const WORD      *m_indices;
   unsigned int     m_numVertices;
   unsigned int     m_numIndices;
   bool m_hitEvent;

private:        // private member functions

   int numIndices;         // only used during loading
   int numVertices;        // only used during loading
#ifdef COMPRESS_MESHES
   int compressedIndices;  // only used during loading
   int compressedVertices; // only used during loading
#endif

   void UpdateEditorView();

   void UpdateAnimation(RenderDevice *pd3dDevice);
   bool BrowseFor3DMeshFile();
   void RenderObject(RenderDevice *pd3dDevice);
   void UpdateTarget(RenderDevice *pd3dDevice);
   void SetupHitObject(Vector<HitObject> * pvho, HitObject * obj);
   void AddHitEdge(Vector<HitObject> * pvho, std::set< std::pair<unsigned, unsigned> >& addedEdges, const unsigned i, const unsigned j, const Vertex3Ds &vi, const Vertex3Ds &vj);

   PropertyPane *m_propVisual;
   PropertyPane *m_propPosition;
   PropertyPane *m_propPhysics;

private:        // private data members

   Vector<HitObject> m_vhoCollidable; // Objects to that may be collide selectable

   // Vertices for editor display
   std::vector<Vertex3Ds> vertices;
   std::vector<float> normals; // only z component actually
   Vertex3D_NoTex2 *transformedVertices;
   float m_moveAnimationOffset;
   bool  m_moveAnimation;
   bool  m_moveDown;

   VertexBuffer *vertexBuffer;
   IndexBuffer *indexBuffer;
   bool vertexBufferRegenerate;
};

#endif // !defined(AFX_HITTARGET_H__A67DE998_7D97_4E03_BE91_55BFD3A48DB6__INCLUDED_)