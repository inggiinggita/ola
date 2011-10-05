/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * EnttecUsbProWidget.h
 * The Enttec USB Pro Widget
 * Copyright (C) 2010 Simon Newton
 */

#ifndef PLUGINS_USBPRO_ENTTECUSBPROWIDGET_H_
#define PLUGINS_USBPRO_ENTTECUSBPROWIDGET_H_

#include <deque>
#include <string>
#include "ola/Callback.h"
#include "ola/DmxBuffer.h"
#include "ola/thread/SchedulerInterface.h"
#include "ola/rdm/QueueingRDMController.h"
#include "ola/rdm/RDMControllerInterface.h"
#include "ola/rdm/UIDSet.h"
#include "plugins/usbpro/BaseUsbProWidget.h"

namespace ola {
namespace plugin {
namespace usbpro {

typedef struct {
  uint8_t firmware;
  uint8_t firmware_high;
  uint8_t break_time;
  uint8_t mab_time;
  uint8_t rate;
} usb_pro_parameters;

typedef ola::SingleUseCallback2<void, bool, const usb_pro_parameters&>
  usb_pro_params_callback;

/*
 * An Enttec DMX USB PRO Widget implementation. We separate the Widget from the
 * implementation so we can leverage the QueueingRDMController when we add RDM
 * support one day.
 */
class EnttecUsbProWidgetImpl: public BaseUsbProWidget {
  // : public ola::rdm::RDMControllerInterface {
  public:
    EnttecUsbProWidgetImpl(ola::thread::SchedulerInterface *scheduler,
                           ola::network::ConnectedDescriptor *descriptor);
    ~EnttecUsbProWidgetImpl();

    void SetDMXCallback(ola::Callback0<void> *callback);
    void Stop();

    bool SendDMX(const DmxBuffer &buffer);
    bool ChangeToReceiveMode(bool change_only);
    const DmxBuffer &FetchDMX() const;

    void GetParameters(usb_pro_params_callback *callback);
    bool SetParameters(uint8_t break_time,
                       uint8_t mab_time,
                       uint8_t rate);

    /*
     * TODO(simon): add RDM support
    void SendRDMRequest(const ola::rdm::RDMRequest *request,
                        ola::rdm::RDMCallback *on_complete);
    void bool RunFullDiscovery(RDMDiscoveryCallback *callback);
    void RunIncrementalDiscovery(RDMDiscoveryCallback *callback);
    bool CheckDiscoveryStatus();
    */

  private:
    ola::thread::SchedulerInterface *m_scheduler;
    bool m_active;
    // ola::thread::timeout_id m_rdm_timeout_id;
    DmxBuffer m_input_buffer;
    ola::Callback0<void> *m_dmx_callback;
    std::deque<usb_pro_params_callback*> m_outstanding_param_callbacks;
    /*
    ola::Callback1<void, const ola::rdm::UIDSet&> *m_uid_set_callback;
    ola::Callback0<void> *m_discovery_callback;
    ola::rdm::RDMCallback *m_rdm_request_callback;
    const ola::rdm::RDMRequest *m_pending_request;
    uint8_t m_transaction_number;
    */

    void HandleMessage(uint8_t label,
                       const uint8_t *data,
                       unsigned int length);

    void HandleParameters(const uint8_t *data, unsigned int length);
    void HandleDMX(const uint8_t *data, unsigned int length);
    void HandleDMXDiff(const uint8_t *data, unsigned int length);

    /*
    bool InDiscoveryMode() const;
    bool SendDiscoveryStart();
    bool SendDiscoveryStat();
    void FetchNextUID();
    void DispatchRequest(const ola::rdm::RDMRequest *request,
                         ola::rdm::RDMCallback *callback);
    void DispatchQueuedGet(const ola::rdm::RDMRequest* request,
                           ola::rdm::RDMCallback *callback);
    void StopDiscovery();

    void HandleRemoteRDMResponse(uint8_t return_code,
                                 const uint8_t *data,
                                 unsigned int length);
    */

    static const uint8_t REPROGRAM_FIRMWARE_LABEL = 2;
    static const uint8_t PARAMETERS_LABEL = 3;
    static const uint8_t SET_PARAMETERS_LABEL = 4;
    static const uint8_t RECEIVED_DMX_LABEL = 5;
    static const uint8_t DMX_RX_MODE_LABEL = 8;
    static const uint8_t DMX_CHANGED_LABEL = 9;
};


/*
 * An Usb Pro Widget
 */
class EnttecUsbProWidget: public SerialWidgetInterface {
  public:
    EnttecUsbProWidget(ola::thread::SchedulerInterface *scheduler,
                       ola::network::ConnectedDescriptor *descriptor,
                       unsigned int queue_size = 20);
    ~EnttecUsbProWidget() {}

    void Stop() { m_impl.Stop(); }

    bool SendDMX(const DmxBuffer &buffer) {
      return m_impl.SendDMX(buffer);
    }

    const DmxBuffer &FetchDMX() const {
      return m_impl.FetchDMX();
    }

    void SetDMXCallback(
        ola::Callback0<void> *callback) {
      m_impl.SetDMXCallback(callback);
    }

    bool ChangeToReceiveMode(bool change_only) {
      return m_impl.ChangeToReceiveMode(change_only);
    }

    void GetParameters(usb_pro_params_callback *callback) {
      m_impl.GetParameters(callback);
    }

    bool SetParameters(uint8_t break_time,
                       uint8_t mab_time,
                       uint8_t rate) {
      return m_impl.SetParameters(break_time, mab_time, rate);
    }

    /*
    void SetUIDListCallback(
        ola::Callback1<void, const ola::rdm::UIDSet&> *callback) {
      m_impl.SetUIDListCallback(callback);
    }

    void SendRDMRequest(const ola::rdm::RDMRequest *request,
                        ola::rdm::RDMCallback *on_complete) {
      m_controller.SendRDMRequest(request, on_complete);
    }

    bool RunFullDiscovery(RDMDiscoveryCallback *callback) {
      m_controller.RunFullDiscovery(callback);
    }

    bool RunIncrementalDiscovery(RDMDiscoveryCallback *callback) {
      m_controller.RunIncrementalDiscovery(callback);
    }
    */

    ola::network::ConnectedDescriptor *GetDescriptor() const {
      return m_impl.GetDescriptor();
    }

    void SetOnRemove(ola::SingleUseCallback0<void> *on_close) {
      m_impl.SetOnRemove(on_close);
    }

    void CloseDescriptor() { m_impl.CloseDescriptor(); }

  private:
    EnttecUsbProWidgetImpl m_impl;
    //  ola::rdm::QueueingRDMController m_controller;

    /*
    void ResumeRDMCommands() {
      m_controller.Resume();
    }
    */
};
}  // usbpro
}  // plugin
}  // ola
#endif  // PLUGINS_USBPRO_ENTTECUSBPROWIDGET_H_