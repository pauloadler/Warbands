import unreal

assets = unreal.EditorAssetLibrary
path = '/Game/Warbands/Animation/ABP_SwordGrip'
source = '/Game/Characters/Mannequins/Anims/Rifle/MF_Rifle_Idle_ADS'
pose = assets.load_asset(source)
assert pose, source
assets.make_directory('/Game/Warbands/Animation')
if assets.does_asset_exist(path):
    raise RuntimeError('Grip asset already exists; inspect it before rebuilding')
factory = unreal.AnimBlueprintFactory()
factory.set_editor_property('target_skeleton', pose.get_editor_property('skeleton'))
bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset('ABP_SwordGrip', '/Game/Warbands/Animation', unreal.AnimBlueprint, factory)
result = unreal.WarbandsEditorBridge.prepare_sword_grip(bp, pose)
unreal.log(result)
assert result.startswith('OK'), result
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
assert assets.save_loaded_asset(bp)
controller = assets.load_asset('/Game/Warbands/Blueprints/BP_WarbandsPlayerController')
defaults = unreal.get_default_object(assets.load_blueprint_class('/Game/Warbands/Blueprints/BP_WarbandsPlayerController'))
defaults.set_editor_property('sword_grip_animation_class', assets.load_blueprint_class(path))
assert assets.save_loaded_asset(controller)
unreal.log('SWORD_GRIP_SETUP_OK')
