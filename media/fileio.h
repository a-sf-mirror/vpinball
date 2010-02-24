
#define FID(A) *(int *)#A

void TitleFromFilename(char *szfilename, char *sztitle);
void ExtensionFromFilename(char *szfilename, char *szextension);
BOOL RawReadFromFile(char *szfilename, int *psize, char **pszout);
void PathFromFilename(char *szfilename, char *szpath);
void TitleAndPathFromFilename(char *szfilename, char *szpath);

class BiffReader;

class ILoadable
	{
public:
	virtual BOOL LoadToken(int id, BiffReader *pbr) = 0;
	};

class BiffWriter
	{
public:
	BiffWriter(IStream *pistream, HCRYPTHASH hcrypthash, HCRYPTKEY hcryptkey);
	HRESULT WriteInt(int id, int value);
	HRESULT WriteString(int id, char *szvalue);
	HRESULT WriteWideString(int id, WCHAR *wzvalue);
	HRESULT WriteBool(int id, BOOL fvalue);
	HRESULT WriteFloat(int id, float value);
	HRESULT WriteStruct(int id, void *pvalue, int size);
	HRESULT WriteTag(int id);

	HRESULT WriteBytes(const void *pv,unsigned long count,unsigned long *foo);

	HRESULT WriteRecordSize(int size);

	IStream *m_pistream;
	HCRYPTHASH m_hcrypthash;
	HCRYPTKEY m_hcryptkey;
	};

class BiffReader
	{
public:
	BiffReader(IStream *pistream, ILoadable *piloadable, void *ppassdata, int version, HCRYPTHASH hcrypthash, HCRYPTKEY hcryptkey);

	HRESULT GetIntNoHash(void *pvalue);
	HRESULT GetInt(void *pvalue);
	HRESULT GetString(char *szvalue);
	HRESULT GetWideString(WCHAR *wzvalue);
	HRESULT GetFloat(float *pvalue);
	HRESULT GetBool(BOOL *pfvalue);
	HRESULT GetStruct(void *pvalue, int size);

	HRESULT ReadBytes(void *pv,unsigned long count,unsigned long *foo);

	HRESULT Load();

	IStream *m_pistream;
	ILoadable *m_piloadable;
	void *m_pdata;
	int m_version;

	int m_bytesinrecordremaining;

	HCRYPTHASH m_hcrypthash;
	HCRYPTKEY m_hcryptkey;
	};

class FastIStream;

class FastIStorage : public IStorage
	{
public:
	FastIStorage();
	virtual ~FastIStorage();

	long __stdcall QueryInterface(const struct _GUID &,void ** );
	unsigned long __stdcall AddRef(void);
	unsigned long __stdcall Release(void);

	long __stdcall CreateStream(const OLECHAR *,unsigned long,unsigned long,unsigned long,struct IStream ** );
	long __stdcall OpenStream(const OLECHAR *,void *,unsigned long,unsigned long,struct IStream ** );
	long __stdcall CreateStorage(const OLECHAR *,unsigned long,unsigned long,unsigned long,struct IStorage ** );
	long __stdcall OpenStorage(const OLECHAR *,struct IStorage *,unsigned long,SNB ,unsigned long,struct IStorage ** );
	long __stdcall CopyTo(unsigned long,const struct _GUID *,SNB ,struct IStorage *);
	long __stdcall MoveElementTo(const OLECHAR *,struct IStorage *,const OLECHAR *,unsigned long);
	long __stdcall Commit(unsigned long);
	long __stdcall Revert(void);
	long __stdcall EnumElements(unsigned long,void *,unsigned long,struct IEnumSTATSTG ** );
	long __stdcall DestroyElement(const OLECHAR *);
	long __stdcall RenameElement(const OLECHAR *,const OLECHAR *);
	long __stdcall SetElementTimes(const OLECHAR *,const struct _FILETIME *,const struct _FILETIME *,const struct _FILETIME *);
	long __stdcall SetClass(const struct _GUID &);
	long __stdcall SetStateBits(unsigned long,unsigned long);
	long __stdcall Stat(struct tagSTATSTG *,unsigned long);

	int m_cref;
	Vector<FastIStorage> m_vstg;
	Vector<FastIStream> m_vstm;

	WCHAR *m_wzName;
	};

class FastIStream : public IStream
	{
public:
	FastIStream();
	virtual ~FastIStream();

	long __stdcall QueryInterface(const struct _GUID &,void ** );
	unsigned long __stdcall AddRef(void);
	unsigned long __stdcall Release(void);
	long __stdcall Read(void *pv,unsigned long count,unsigned long *foo);
	long __stdcall Write(const void *pv,unsigned long count,unsigned long *foo);
	long __stdcall Seek(union _LARGE_INTEGER,unsigned long,union _ULARGE_INTEGER *);
	long __stdcall SetSize(union _ULARGE_INTEGER);
	long __stdcall CopyTo(struct IStream *,union _ULARGE_INTEGER,union _ULARGE_INTEGER *,union _ULARGE_INTEGER *);
	long __stdcall Commit(unsigned long);
	long __stdcall Revert(void);

	long __stdcall LockRegion(union _ULARGE_INTEGER,union _ULARGE_INTEGER,unsigned long);
	long __stdcall UnlockRegion(union _ULARGE_INTEGER,union _ULARGE_INTEGER,unsigned long);
	long __stdcall Stat(struct tagSTATSTG *,unsigned long);
	long __stdcall Clone(struct IStream ** );

	void SetSize(unsigned int i);

	int m_cref;

	unsigned int		m_cMax;		// Number of elements allocated
	unsigned int		m_cSeek;	// Last element used
	unsigned int		m_cSize;	// Size of stream
	char	*m_rg;		// Data buffer

	WCHAR *m_wzName;
	};
