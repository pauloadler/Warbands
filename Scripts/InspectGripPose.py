import unreal
pose = unreal.EditorAssetLibrary.load_asset('/Game/Characters/Mannequins/Anims/Rifle/MF_Rifle_Idle_ADS')
for name in ['index_01_r', 'index_02_r', 'index_03_r', 'middle_01_r', 'middle_02_r', 'middle_03_r']:
    transform = unreal.AnimationLibrary.get_bone_pose_for_time(pose, name, 0.0, False)
    unreal.log('GRIP_BONE ' + name + ' ' + str(transform) + ' rotation=' + str(transform.rotation.rotator()))
