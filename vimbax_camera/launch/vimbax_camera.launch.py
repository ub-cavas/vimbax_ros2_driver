# Copyright (c) 2024 Allied Vision Technologies GmbH. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#
#    * Neither the name of the Allied Vision Technologies GmbH nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.


from launch_ros.actions import Node
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            'camera_id',
            default_value='',
            description='ID of the camera device'
        ),
        DeclareLaunchArgument(
            'camera_frame_id',
            default_value='camera_link',
            description='Frame ID for the camera'
        ),
        DeclareLaunchArgument(
            'settings_file',
            default_value='',
            description='Path to XML settings file to load on startup. '
                        'The file must point to a valid file on the system.'
        ),
        DeclareLaunchArgument(
            'camera_info_url',
            default_value='',
            description='Path to camera info file to load on startup. '
                        'The file must point to a valid file on the system.'
        ),

        Node(
            package='vimbax_camera',
            namespace='vimbax_camera',
            executable='vimbax_camera_node',
            name='vimbax_camera_test',
            parameters=[{
                'camera_id': LaunchConfiguration('camera_id'),
                'camera_frame_id': LaunchConfiguration('camera_frame_id'),
                'settings_file': LaunchConfiguration('settings_file'),
                'camera_info_url': LaunchConfiguration('camera_info_url'),
                'use_ros_time': True,
                'autostream': 1,
            }],
            remappings=[]
        )
    ])
