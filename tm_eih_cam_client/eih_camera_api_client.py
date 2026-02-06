"""EIH Camera API Client

This module provides a Python gRPC client for the TM Robot EIH camera API.
"""

import grpc
import logging
from google.protobuf import empty_pb2
from typing import List, Optional, Tuple
from enum import IntEnum

# Import generated protobuf modules
try:
    from . import EIHCamera_pb2
    from . import EIHCameraAPI_pb2
    from . import EIHCameraAPI_pb2_grpc
except ImportError:
    import EIHCamera_pb2
    import EIHCameraAPI_pb2
    import EIHCameraAPI_pb2_grpc


# Module logger
logger = logging.getLogger(__name__)


class StatusCode(IntEnum):
    """Status codes for gRPC operations"""
    FAIL = -1
    SUCCESS = 0


class GrpcResult:
    """Result object for gRPC operations"""
    def __init__(self):
        self.status = StatusCode.FAIL
        self.error_message = ""


class EIHCameraApiClient:
    """gRPC client for EIH Camera API"""
    
    def __init__(self, server_address: str):
        """Initialize the gRPC client
        
        Args:
            server_address: Server address in format "host:port"
        """
        # Create channel with unlimited message size
        options = [
            ('grpc.max_receive_message_length', -1),
            ('grpc.max_send_message_length', -1),
        ]
        self.channel = grpc.insecure_channel(server_address, options=options)
        self.stub = EIHCameraAPI_pb2_grpc.EIHCameraApiStub(self.channel)
    
    def is_camera_connected(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Check if camera is connected
        
        Returns:
            Tuple of (GrpcResult, camera connection info dict or None)
        """
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.isCameraConnected(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'is_camera_connected': response.isCameraConnected,
                'connection_message': response.connection_message
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def get_intrinsics(self) -> Tuple[GrpcResult, Optional[List[dict]]]:
        """Get camera intrinsics for different focus and resolution settings
        
        Returns:
            Tuple of (GrpcResult, list of intrinsics dicts or None)
        """
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getIntrinsics(request)
            
            intrinsics_list = []
            for intrinsics in response.cam_intrinsics:
                intrinsics_dict = {
                    'focus_value': intrinsics.FocusValue,
                    'image_width': intrinsics.ImageWidth,
                    'image_height': intrinsics.ImageHeight,
                    'camera_matrix': {
                        'matrix_00': intrinsics.CameraMatrix.matrix_00,
                        'matrix_01': intrinsics.CameraMatrix.matrix_01,
                        'matrix_02': intrinsics.CameraMatrix.matrix_02,
                        'matrix_10': intrinsics.CameraMatrix.matrix_10,
                        'matrix_11': intrinsics.CameraMatrix.matrix_11,
                        'matrix_12': intrinsics.CameraMatrix.matrix_12,
                        'matrix_20': intrinsics.CameraMatrix.matrix_20,
                        'matrix_21': intrinsics.CameraMatrix.matrix_21,
                        'matrix_22': intrinsics.CameraMatrix.matrix_22,
                    },
                    'distortion_coefficients': {
                        'coefficient_00': intrinsics.DistortionCoefficients.coefficient_00,
                        'coefficient_10': intrinsics.DistortionCoefficients.coefficient_10,
                        'coefficient_20': intrinsics.DistortionCoefficients.coefficient_20,
                        'coefficient_30': intrinsics.DistortionCoefficients.coefficient_30,
                        'coefficient_40': intrinsics.DistortionCoefficients.coefficient_40,
                    }
                }
                intrinsics_list.append(intrinsics_dict)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            return result, intrinsics_list
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def get_hand_eye_parameters(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Get hand-eye calibration parameters
        
        Returns:
            Tuple of (GrpcResult, hand-eye parameters dict or None)
        """
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getHandEyeParameters(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'handeye_x': response.HandEyeArray.handeye_x,
                'handeye_y': response.HandEyeArray.handeye_y,
                'handeye_z': response.HandEyeArray.handeye_z,
                'handeye_rx': response.HandEyeArray.handeye_rx,
                'handeye_ry': response.HandEyeArray.handeye_ry,
                'handeye_rz': response.HandEyeArray.handeye_rz,
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def get_image_data(self) -> Tuple[GrpcResult, Optional[bytes]]:
        """Get encoded image data
        
        Returns:
            Tuple of (GrpcResult, image bytes or None)
        """
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getImageData(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            return result, response.EncodeString
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def get_image_configuration(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Get image configuration
        
        Returns:
            Tuple of (GrpcResult, image config dict or None)
        """
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getImageConfiguration(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'image_type': response.ImageType,
                'image_size': response.ImageSize,
                'image_width': response.ImageWidth,
                'image_height': response.ImageHeight,
                'pixel_format': response.PixelFormat,
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def get_capturing_settings(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Get all capturing settings
        
        Returns:
            Tuple of (GrpcResult, capturing settings dict or None)
        """
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getCapturingSettings(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'shutter_time': {
                    'current_value': response.ShutterTime.CurrentValue,
                    'min_value': response.ShutterTime.MinValue,
                    'max_value': response.ShutterTime.MaxValue,
                },
                'gain': {
                    'current_value': response.Gain.CurrentValue,
                    'min_value': response.Gain.MinValue,
                    'max_value': response.Gain.MaxValue,
                },
                'white_balance': {
                    'red_ratio': {
                        'current_value': response.WhiteBalance.red_ratio.CurrentValue,
                        'min_value': response.WhiteBalance.red_ratio.MinValue,
                        'max_value': response.WhiteBalance.red_ratio.MaxValue,
                    },
                    'green_ratio': {
                        'current_value': response.WhiteBalance.green_ratio.CurrentValue,
                        'min_value': response.WhiteBalance.green_ratio.MinValue,
                        'max_value': response.WhiteBalance.green_ratio.MaxValue,
                    },
                    'blue_ratio': {
                        'current_value': response.WhiteBalance.blue_ratio.CurrentValue,
                        'min_value': response.WhiteBalance.blue_ratio.MinValue,
                        'max_value': response.WhiteBalance.blue_ratio.MaxValue,
                    },
                },
                'focus': {
                    'current_value': response.Focus.CurrentValue,
                    'min_value': response.Focus.MinValue,
                    'max_value': response.Focus.MaxValue,
                },
                'image_size': response.ImageSize,
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def set_capturing_settings(self, shutter_time: int = -1, gain: int = -1,
                              wb_redratio: int = -1, wb_greenratio: int = -1,
                              wb_blueratio: int = -1, focus: int = -1,
                              image_size: str = "") -> GrpcResult:
        """Set capturing settings
        
        Args:
            shutter_time: Shutter time (134~66371)
            gain: Gain (0~100)
            wb_redratio: White balance red ratio (22~121)
            wb_greenratio: White balance green ratio (1)
            wb_blueratio: White balance blue ratio (34~94)
            focus: Focus (0~8)
            image_size: Image size (1M or 5M)
        
        Returns:
            GrpcResult
        """
        result = GrpcResult()
        try:
            request = EIHCameraAPI_pb2.setCapturingSettingsRequest(
                ShutterTime=shutter_time,
                Gain=gain,
                wb_redratio=wb_redratio,
                wb_greenratio=wb_greenratio,
                wb_blueratio=wb_blueratio,
                Focus=focus,
                ImageSize=image_size
            )
            self.stub.setCapturingSettings(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            logger.info("Capturing settings updated successfully")
            return result
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            logger.error(f"RPC failed: {e}")
            return result
    
    def get_shutter_time(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Get shutter time setting"""
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getShutterTime(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'current_value': response.shuttertime.CurrentValue,
                'min_value': response.shuttertime.MinValue,
                'max_value': response.shuttertime.MaxValue,
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def set_shutter_time(self, shutter_time: int) -> GrpcResult:
        """Set shutter time"""
        result = GrpcResult()
        try:
            request = EIHCameraAPI_pb2.setShutterTimeRequest(shuttertime=shutter_time)
            self.stub.setShutterTime(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            logger.info("Shutter time set successfully")
            return result
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            logger.error(f"RPC failed: {e}")
            return result
    
    def get_gain(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Get gain setting"""
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getGain(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'current_value': response.gain.CurrentValue,
                'min_value': response.gain.MinValue,
                'max_value': response.gain.MaxValue,
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def set_gain(self, gain: int) -> GrpcResult:
        """Set gain"""
        result = GrpcResult()
        try:
            request = EIHCameraAPI_pb2.setGainRequest(gain=gain)
            self.stub.setGain(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            logger.info("Gain set successfully")
            return result
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            logger.error(f"RPC failed: {e}")
            return result
    
    def get_white_balance(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Get white balance settings"""
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getWhiteBalance(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'red_ratio': {
                    'current_value': response.whitebalance.red_ratio.CurrentValue,
                    'min_value': response.whitebalance.red_ratio.MinValue,
                    'max_value': response.whitebalance.red_ratio.MaxValue,
                },
                'green_ratio': {
                    'current_value': response.whitebalance.green_ratio.CurrentValue,
                    'min_value': response.whitebalance.green_ratio.MinValue,
                    'max_value': response.whitebalance.green_ratio.MaxValue,
                },
                'blue_ratio': {
                    'current_value': response.whitebalance.blue_ratio.CurrentValue,
                    'min_value': response.whitebalance.blue_ratio.MinValue,
                    'max_value': response.whitebalance.blue_ratio.MaxValue,
                },
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def set_white_balance(self, wb_redratio: int, wb_greenratio: int, 
                         wb_blueratio: int) -> GrpcResult:
        """Set white balance"""
        result = GrpcResult()
        try:
            request = EIHCameraAPI_pb2.setWhiteBalanceRequest(
                wb_redratio=wb_redratio,
                wb_greenratio=wb_greenratio,
                wb_blueratio=wb_blueratio
            )
            self.stub.setWhiteBalance(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            logger.info("White Balance set successfully")
            return result
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            logger.error(f"RPC failed: {e}")
            return result
    
    def get_focus(self) -> Tuple[GrpcResult, Optional[dict]]:
        """Get focus setting"""
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getFocus(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            
            return result, {
                'current_value': response.focus.CurrentValue,
                'min_value': response.focus.MinValue,
                'max_value': response.focus.MaxValue,
            }
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def set_focus(self, focus: int) -> GrpcResult:
        """Set focus"""
        result = GrpcResult()
        try:
            request = EIHCameraAPI_pb2.setFocusRequest(focus=focus)
            self.stub.setFocus(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            logger.info("Focus set successfully")
            return result
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            logger.error(f"RPC failed: {e}")
            return result
    
    def get_image_size(self) -> Tuple[GrpcResult, Optional[str]]:
        """Get image size setting"""
        result = GrpcResult()
        try:
            request = empty_pb2.Empty()
            response = self.stub.getImageSize(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            return result, response.imagesize
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            return result, None
    
    def set_image_size(self, image_size: str) -> GrpcResult:
        """Set image size"""
        result = GrpcResult()
        try:
            request = EIHCameraAPI_pb2.setImageSizeRequest(imagesize=image_size)
            self.stub.setImageSize(request)
            
            result.status = StatusCode.SUCCESS
            result.error_message = ""
            logger.info("Image Size set successfully")
            return result
        except grpc.RpcError as e:
            result.status = StatusCode.FAIL
            result.error_message = str(e)
            logger.error(f"RPC failed: {e}")
            return result
    
    def __del__(self):
        """Clean up gRPC channel"""
        if hasattr(self, 'channel'):
            self.channel.close()
