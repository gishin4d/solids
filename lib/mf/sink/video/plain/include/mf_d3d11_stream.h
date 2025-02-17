#ifndef _SLD_MF_D3D11_STREAM_H_
#define _SLD_MF_D3D11_STREAM_H_

#include <mf_base.h>
#include <mf_sink_async_operation.h>
#include "mf_d3d11_renderer.h"

namespace solids
{
    namespace lib
    {
        namespace mf
        {
            namespace sink
            {
                namespace video
                {
                    namespace plain
                    {
                        class stream
                            : solids::lib::mf::base
                            , solids::lib::mf::refcount_object
                            , public IMFStreamSink
                            , public IMFMediaTypeHandler
                            , public IMFGetService
                            , public solids::lib::mf::scheduler_callback_t
                            , public solids::lib::mf::attributes<IMFAttributes>
                        {
                        public:
                            typedef struct _state_t
                            {
                                static const int32_t type_not_set = 0;
                                static const int32_t ready = 1;
                                static const int32_t started = 2;
                                static const int32_t paused = 3;
                                static const int32_t stopped = 4;
                                static const int32_t count = 5;
                            } state_t;

                            typedef struct _consume_state_t
                            {
                                static const int32_t drop_samples = 0;
                                static const int32_t process_samples = 1;
                            } consume_state_t;

                            typedef struct _fraction_t
                            {
                                DWORD numerator;
                                DWORD denominator;
                            } fraction_t;

                            typedef struct _format_entry_t
                            {
                                GUID            sub_type;
                                DXGI_FORMAT     dxgi_format;
                            } format_entry_t;

                            stream(DWORD dwStreamId, solids::lib::mf::critical_section * cs, solids::lib::mf::scheduler * sched);
                            virtual ~stream(void);

                            // IUnknown
                            STDMETHODIMP_(ULONG) AddRef(void);
                            STDMETHODIMP QueryInterface(REFIID iid, __RPC__deref_out _Result_nullonfailure_ void** ppv);
                            STDMETHODIMP_(ULONG) Release(void);

                            // IMFStreamSink
                            STDMETHODIMP Flush(void);
                            STDMETHODIMP GetIdentifier(__RPC__out DWORD* pdwIdentifier);
                            STDMETHODIMP GetMediaSink(__RPC__deref_out_opt IMFMediaSink** ppMediaSink);
                            STDMETHODIMP GetMediaTypeHandler(__RPC__deref_out_opt IMFMediaTypeHandler** ppHandler);
                            STDMETHODIMP PlaceMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType, __RPC__in const PROPVARIANT* pvarMarkerValue, __RPC__in const PROPVARIANT* pvarContextValue);
                            STDMETHODIMP ProcessSample(__RPC__in_opt IMFSample* pSample);

                            // IMFMediaEventGenerator (from IMFStreamSink)
                            STDMETHODIMP BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState);
                            STDMETHODIMP EndGetEvent(IMFAsyncResult* pResult, _Out_ IMFMediaEvent** ppEvent);
                            STDMETHODIMP GetEvent(DWORD dwFlags, __RPC__deref_out_opt IMFMediaEvent** ppEvent);
                            STDMETHODIMP QueueEvent(MediaEventType met, __RPC__in REFGUID guidExtendedType, HRESULT hrStatus, __RPC__in_opt const PROPVARIANT* pvValue);

                            // IMFMediaTypeHandler
                            STDMETHODIMP GetCurrentMediaType(_Outptr_ IMFMediaType** ppMediaType);
                            STDMETHODIMP GetMajorType(__RPC__out GUID* pguidMajorType);
                            STDMETHODIMP GetMediaTypeByIndex(DWORD dwIndex, _Outptr_ IMFMediaType** ppType);
                            STDMETHODIMP GetMediaTypeCount(__RPC__out DWORD* pdwTypeCount);
                            STDMETHODIMP IsMediaTypeSupported(IMFMediaType* pMediaType, _Outptr_opt_result_maybenull_ IMFMediaType** ppMediaType);
                            STDMETHODIMP SetCurrentMediaType(IMFMediaType* pMediaType);

                            // IMFGetService
                            STDMETHODIMP GetService(__RPC__in REFGUID guidService, __RPC__in REFIID riid, __RPC__deref_out_opt LPVOID* ppvObject);

                            // SchedulerCallback
                            HRESULT process(IMFSample * sample);

                            HRESULT get_max_rate(BOOL fThin, float * pflRate);
                            HRESULT initialize(IMFMediaSink* pParent, solids::lib::mf::sink::video::plain::renderer * presenter);
                            HRESULT release(void);
                            BOOL    is_active(void) const;
                            HRESULT pause(void);
                            HRESULT preroll(void);
                            HRESULT restart(void);
                            HRESULT start(MFTIME start);
                            HRESULT stop(void);

                        private:
                            HRESULT dispatch_process_sample(solids::lib::mf::sink::async_operation * pOp);
                            HRESULT check_shutdown(void) const;
                            HRESULT get_fps(IMFMediaType* pType, MFRatio* pRatio);
                            BOOL    need_more_samples(void);
                            HRESULT dispatch_workitem_cb(IMFAsyncResult* pAsyncResult);
                            HRESULT process_samples_from_queue(int32_t consumeData);
                            HRESULT queue_async_operation(int32_t op);
                            HRESULT request_samples(void);
                            HRESULT send_marker_event(IMarker* pMarker, int32_t consumeData);
                            HRESULT validate_operation(int32_t op);

                        private:
                            static GUID const* const                                _video_formats[];
                            static const DWORD                                      _nvideo_formats;
                            static const MFRatio                                    _default_fps;

                            static const format_entry_t                             _dxgi_format_mapping[];
                            static BOOL                                             _valid_state_mat[state_t::count][async_operation::type_t::count];

                            const DWORD                                             _stream_id;
                            solids::lib::mf::critical_section *                     _lock;
                            int32_t                                                 _state;
                            BOOL                                                    _is_shutdown;
                            DWORD                                                   _work_queue_id;
                            solids::lib::mf::async_callback<solids::lib::mf::sink::video::plain::stream> _work_queue_cb;
                            int32_t                                                 _consume_data;
                            MFTIME                                                  _start_time;
                            DWORD                                                   _nwritten;
                            DWORD                                                   _noutstanding_sample_requests;
                            IMFMediaSink *                                          _media;
                            IMFMediaEventQueue *                                    _event_queue;
                            IMFByteStream *                                         _byte_stream;
                            solids::lib::mf::sink::video::plain::renderer *         _renderer;
                            solids::lib::mf::scheduler *                            _scheduler;
                            IMFMediaType *                                          _current_type;
                            BOOL                                                    _prerolling;
                            BOOL                                                    _waiting_for_on_clock_start;
                            solids::lib::mf::thread_safe_queue<IUnknown>            _samples_to_process;             // Queue to hold samples and markers. Applies to: ProcessSample, PlaceMarker
                            UINT32                                                  _uninterlace_mode;
                            solids::lib::mf::sink::video::plain::stream::fraction_t _image_bytes_pp;
                            DXGI_FORMAT                                             _dxgi_format;
                        };
                    };
                };
            };
        };
    };
};




#endif