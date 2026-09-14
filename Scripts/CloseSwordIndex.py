import unreal

a = unreal.EditorAssetLibrary
source = a.load_asset('/Game/Characters/Mannequins/Anims/Rifle/MF_Rifle_Idle_ADS')
path = '/Game/Warbands/Animation/AS_SwordGrip'
pose = a.load_asset(path) if a.does_asset_exist(path) else a.duplicate_asset(source.get_path_name(), path)
assert pose
controller = pose.get_editor_property("controller")
# The evaluator samples time zero. Keep the source translations and bone lengths.
for name, yaw in [('index_01_r', -50.0), ('index_02_r', -75.0), ('index_03_r', -60.0)]:
    original = unreal.AnimationLibrary.get_bone_pose_for_time(source, name, 0.0, False)
    rotation = original.rotation.rotator()
    rotation.yaw = yaw
    assert controller.set_bone_track_keys(name, [original.translation], [rotation.quaternion()], [original.scale3d])
assert a.save_loaded_asset(pose)
bp = a.load_asset('/Game/Warbands/Animation/ABP_SwordGrip')
assert unreal.WarbandsEditorBridge.set_sword_grip_pose(bp, pose)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
assert a.save_loaded_asset(bp)
for name in ['index_01_r', 'index_02_r', 'index_03_r']:
    result = unreal.AnimationLibrary.get_bone_pose_for_time(pose, name, 0.0, False)
    unreal.log('CLOSED_INDEX ' + name + ' ' + str(result.rotation.rotator()))
unreal.log('SWORD_INDEX_CLOSED_OK')

