# Copyright 2024 Allied Vision Technologies GmbH. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import pytest

import rclpy
import rclpy.node
import rclpy.executors

import launch_pytest
import launch

from launch.actions import ExecuteProcess

from launch_ros.actions import Node

from threading import Thread

import queue
import time
import random
import string

from sensor_msgs.msg import Image


class TestNode(rclpy.node.Node):
    __test__ = False

    def __init__(self, name, camera_node_name):
        rclpy.node.Node.__init__(self, name)
        self.image_queue = queue.Queue()
        self._camera_node_name = camera_node_name

        self.ros_spin_thread = Thread(target=lambda node: rclpy.spin(node), args=(self, ))
        self.ros_spin_thread.start()

    def subscribe_image_raw(self):

        def callback(image):
            self.image_queue.put(image)

        self.image_subscribtion = self.create_subscription(
            Image, f"{self._camera_node_name}/image_raw", callback, 10
            )

    def clear_queue(self):
        while not self.image_queue.empty():
            self.image_queue.get()

    def unsubscribe_image_raw(self):
        assert self.destroy_subscription(self.image_subscribtion)
        time.sleep(0.1)
        self.clear_queue()

    def wait_for_frame(self, timeout: float) -> Image:
        return self.image_queue.get(block=True, timeout=timeout)

    def call_service_sync(self, service, request):
        return service.call(request)

    def camera_node_name(self):
        return self._camera_node_name


@pytest.fixture
def node_test_id():
    return ''.join(random.choices(string.ascii_lowercase + string.digits, k=8))


@pytest.fixture
def camera_test_node_name(node_test_id):
    return f"vimbax_camera_pytest_{node_test_id}"


@launch_pytest.fixture
def vimbax_camera_node(camera_test_node_name):
    return launch.LaunchDescription([
        ExecuteProcess(
            cmd=[
                "ros2",
                "node",
                "list",
                "--all"
            ],
            shell=True,
            output='both',
        ),
        Node(
            package='vimbax_camera',
            namespace=camera_test_node_name,
            executable='vimbax_camera_node',
            name=camera_test_node_name
        ),
        # Tell launch when to start the test
        # If no ReadyToTest action is added, one will be appended automatically.
        launch_pytest.actions.ReadyToTest()
    ])


@pytest.fixture
def test_node(node_test_id, camera_test_node_name):
    rclpy.init()
    yield TestNode(f"_test_node_{node_test_id}", camera_test_node_name)
    rclpy.shutdown()
