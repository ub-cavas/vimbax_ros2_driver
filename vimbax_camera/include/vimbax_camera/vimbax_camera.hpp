// Copyright 2024 Allied Vision Technologies GmbH. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef VIMBAX_CAMERA__VIMBAX_CAMERA_HPP_
#define VIMBAX_CAMERA__VIMBAX_CAMERA_HPP_

#include <string>
#include <memory>
#include <functional>
#include <optional>
#include <vector>
#include <utility>
#include <unordered_map>

#include <rclcpp/logger.hpp>
#include <sensor_msgs/image_encodings.hpp>
#include <sensor_msgs/msg/image.hpp>

#include <vimbax_camera/result.hpp>
#include <vimbax_camera/loader/vmbc_api.hpp>
#include <vimbax_camera/vimbax_camera_helper.hpp>


namespace vimbax_camera
{
class VimbaXCamera : public std::enable_shared_from_this<VimbaXCamera>
{
public:
  class Frame : public sensor_msgs::msg::Image, public std::enable_shared_from_this<Frame>
  {
    /* *INDENT-OFF* */
  public:
    /* *INDENT-ON* */
    static result<std::shared_ptr<Frame>> create(
      std::shared_ptr<VimbaXCamera> camera, size_t size, size_t alignment = 1);

    ~Frame();

    Frame(const Frame &) = delete;
    Frame & operator=(const Frame &) = delete;

    void set_callback(std::function<void(std::shared_ptr<Frame>)> callback);

    int32_t queue() const;

    std::string get_image_encoding() const;

    int64_t get_frame_id() const;
    /* *INDENT-OFF* */
  private:
    /* *INDENT-ON* */
    enum class AllocationMode
    {
      kByImage,
      kByTl,
    };

    static void vmb_frame_callback(const VmbHandle_t, const VmbHandle_t, VmbFrame_t * frame);
    void on_frame_ready();
    void transform();
    uint64_t timestamp_to_ns(uint64_t timestamp);

    Frame(std::shared_ptr<VimbaXCamera> camera, AllocationMode allocationMode);

    std::function<void(std::shared_ptr<Frame>)> callback_;
    std::weak_ptr<VimbaXCamera> camera_;
    VmbFrame vmb_frame_;

    AllocationMode allocation_mode_;
  };


  struct Info
  {
    std::string display_name;
    std::string model_name;
    std::string firmware_version;
    std::string device_id;
    std::string device_user_id;
    std::string device_serial_number;
    std::string interface_id;
    std::string transport_layer_id;
    std::optional<std::string> ip_address{std::nullopt};
    std::optional<std::string> mac_address{std::nullopt};
    bool streaming;
    uint32_t width;
    uint32_t height;
    double frame_rate;
    std::string pixel_format;
    std::string trigger_mode;
    std::string trigger_source;
  };

  static std::shared_ptr<VimbaXCamera> open(
    std::shared_ptr<VmbCAPI> api,
    const std::string & name = {});

  ~VimbaXCamera();

  VimbaXCamera(const VimbaXCamera &) = delete;
  VimbaXCamera & operator=(const VimbaXCamera &) = delete;

  result<void> start_streaming(
    int bufferCount,
    std::function<void(std::shared_ptr<Frame>)> onFrame,
    bool startAcquisition = true);
  result<void> stop_streaming();

  bool is_alive();

  // Feature access
  result<std::vector<std::string>> features_list_get(void) const;

  result<bool> feature_command_is_done(
    const std::string_view & name) const;
  result<void> feature_command_run(
    const std::string_view & name) const;

  result<int64_t> feature_int_get(
    const std::string_view & name) const;
  result<void> feature_int_set(
    const std::string_view & name, const int64_t value) const;
  result<std::array<int64_t, 3>> feature_int_info_get(
    const std::string_view & name) const;

  result<_Float64> feature_float_get(
    const std::string_view & name) const;
  result<void> feature_float_set(
    const std::string_view & name, const _Float64 value) const;
  result<feature_float_info> feature_float_info_get(
    const std::string_view & name) const;

  result<std::string> feature_string_get(
    const std::string_view & name) const;
  result<void> feature_string_set(
    const std::string_view & name, const std::string_view value) const;
  result<uint32_t> feature_string_info_get(
    const std::string_view & name) const;

  result<bool> feature_bool_get(
    const std::string_view & name) const;
  result<void> feature_bool_set(
    const std::string_view & name, const bool value) const;

  result<std::string> feature_enum_get(
    const std::string_view & name) const;
  result<void> feature_enum_set(
    const std::string_view & name, const std::string_view & value) const;
  result<std::array<std::vector<std::string>, 2>> feature_enum_info_get(
    const std::string_view & name) const;
  result<int64_t> feature_enum_as_int_get(
    const std::string_view & name, const std::string_view & option) const;
  result<std::string> feature_enum_as_string_get(
    const std::string_view & name, const int64_t value) const;

  result<std::vector<unsigned char>> feature_raw_get(
    const std::string_view & name) const;
  result<void> feature_raw_set(
    const std::string_view & name, const std::vector<uint8_t> buffer) const;
  result<uint32_t> feature_raw_info_get(
    const std::string_view & name) const;

  result<std::array<bool, 2>> feature_access_mode_get(
    const std::string_view & name) const;

  result<std::vector<feature_info>> feature_info_query_list(
    const std::vector<std::string> & names) const;


  result<VmbPixelFormatType> get_pixel_format() const;

  result<VmbCameraInfo> query_camera_info() const;

  result<VmbFeatureInfo> feature_info_query(
    const std::string_view & name) const;

  result<void> settings_load(const std::string_view & fileName);

  result<void> settings_save(const std::string_view & fileName);

  result<Info> camera_info_get() const;

  bool is_streaming() const;

  result<void> feature_invalidation_register(
    const std::string_view & name,
    std::function<void(const std::string &)> callback);

  result<void> feature_invalidation_unregister(const std::string_view & name);

  using EventMetaDataList = std::vector<std::pair<std::string, std::string>>;

  result<EventMetaDataList> get_event_meta_data(const std::string_view & name);

private:
  explicit VimbaXCamera(std::shared_ptr<VmbCAPI> api, VmbHandle_t cameraHandle);

  static void on_feature_invalidation(VmbHandle_t, const char * name, void * context);

  result<void> feature_command_run(const std::string_view & name, VmbHandle_t handle) const;

  result<int64_t> feature_int_get(const std::string_view & name, VmbHandle_t handle) const;

  result<_Float64> feature_float_get(const std::string_view & name, VmbHandle_t handle) const;

  result<std::string> feature_enum_get(const std::string_view & name, VmbHandle_t handle) const;

  result<std::string> feature_string_get(const std::string_view & name, VmbHandle_t handle) const;

  VmbFeaturePersistSettings get_default_feature_persist_settings() const;

  std::shared_ptr<VmbCAPI> api_;
  VmbHandle_t camera_handle_;
  std::vector<std::shared_ptr<Frame>> frames_;
  bool streaming_{false};
  VmbCameraInfo camera_info_;
  std::optional<uint64_t> timestamp_frequency_;
  std::unordered_map<std::string, std::function<void(const std::string &)>> invalidation_callbacks_;

  std::mutex invalidation_callbacks_mutex_{};

  std::unordered_map<std::string, VmbFeatureInfo> feature_info_map_;
  std::unordered_multimap<std::string, std::string> feature_category_map_;
};

}  // namespace vimbax_camera

#endif  // VIMBAX_CAMERA__VIMBAX_CAMERA_HPP_
