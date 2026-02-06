from setuptools import setup, find_packages

package_name = 'tm_eih_cam_client'

setup(
    name=package_name,
    version='0.1.0',
    packages=find_packages(),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=[
        'setuptools',
        'grpcio>=1.60.0',
        'grpcio-tools>=1.60.0',
        'protobuf>=4.21.0',
    ],
    zip_safe=True,
    maintainer='steven.zheng',
    maintainer_email='steven.zheng@tm-robot.com',
    description='TM EIH camera gRPC client for ROS2',
    license='BSD-3-Clause',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'eih_client_pub = tm_eih_cam_client.eih_client_pub_node:main',
            'image_sub = tm_eih_cam_client.image_sub_node:main',
            'eih_client_test_get = tm_eih_cam_client.eih_client_test_get:main',
            'eih_client_test_set = tm_eih_cam_client.eih_client_test_set:main',
        ],
    },
)
