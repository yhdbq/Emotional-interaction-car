/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_WIFI_DEVICE_SERVICE_IMPL_H
#define OHOS_WIFI_DEVICE_SERVICE_IMPL_H

#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "i_wifi_device_callback.h"
#ifdef OHOS_ARCH_LITE
#include "wifi_device_stub_lite.h"
#else
#include "system_ability.h"
#include "wifi_device_stub.h"
#include "iremote_object.h"
#endif

namespace OHOS {
namespace Wifi {
enum ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};

#ifdef OHOS_ARCH_LITE
class WifiDeviceServiceImpl : public WifiDeviceStub {
#else
class WifiDeviceServiceImpl : public SystemAbility, public WifiDeviceStub {
DECLARE_SYSTEM_ABILITY(WifiDeviceServiceImpl);
#endif
public:
    WifiDeviceServiceImpl();
    virtual ~WifiDeviceServiceImpl();

#ifdef OHOS_ARCH_LITE
    static std::shared_ptr<WifiDeviceServiceImpl> GetInstance();

    void OnStart();
    void OnStop();
#else
    static sptr<WifiDeviceServiceImpl> GetInstance();

    void OnStart() override;
    void OnStop() override;
#endif

    ErrCode EnableWifi() override;

    ErrCode DisableWifi() override;

    ErrCode AddDeviceConfig(const WifiDeviceConfig &config, int &result) override;

    ErrCode RemoveDevice(int networkId) override;

    ErrCode RemoveAllDevice() override;

    ErrCode GetDeviceConfigs(std::vector<WifiDeviceConfig> &result) override;

    ErrCode EnableDeviceConfig(int networkId, bool attemptEnable) override;

    ErrCode DisableDeviceConfig(int networkId) override;

    ErrCode ConnectToNetwork(int networkId) override;

    ErrCode ConnectToDevice(const WifiDeviceConfig &config) override;

    ErrCode ReConnect() override;

    ErrCode ReAssociate(void) override;

    ErrCode Disconnect(void) override;

    ErrCode StartWps(const WpsConfig &config) override;

    ErrCode CancelWps(void) override;

    ErrCode IsWifiActive(bool &bActive) override;

    ErrCode GetWifiState(int &state) override;

    ErrCode GetLinkedInfo(WifiLinkedInfo &info) override;

    ErrCode GetIpInfo(IpInfo &info) override;

    ErrCode SetCountryCode(const std::string &countryCode) override;

    ErrCode GetCountryCode(std::string &countryCode) override;

#ifdef OHOS_ARCH_LITE
    ErrCode RegisterCallBack(const std::shared_ptr<IWifiDeviceCallBack> &callback) override;
#else
    ErrCode RegisterCallBack(const sptr<IWifiDeviceCallBack> &callback) override;
#endif

    ErrCode GetSignalLevel(const int &rssi, const int &band, int &level) override;

    ErrCode GetSupportedFeatures(long &features) override;

    ErrCode GetDeviceMacAddress(std::string &result) override;

private:
    bool Init();
    ErrCode CheckCanEnableWifi(void);
    bool IsStaServiceRunning();
    bool IsScanServiceRunning();

private:
#ifdef OHOS_ARCH_LITE
    static std::shared_ptr<WifiDeviceServiceImpl> g_instance;
#else
    static sptr<WifiDeviceServiceImpl> g_instance;
#endif
    static std::mutex g_instanceLock;
    bool mPublishFlag;
    ServiceRunningState mState;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
