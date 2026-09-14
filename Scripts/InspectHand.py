import unreal
a = unreal.EditorAssetLibrary
c = unreal.get_default_object(a.load_blueprint_class('/Game/Warbands/Blueprints/Characters/BP_PlayerCharacter'))
m = c.get_component_by_class(unreal.SkeletalMeshComponent)
unreal.log('HAND_INSPECT mesh=' + str(m.get_editor_property('skeletal_mesh_asset')) + ' anim=' + str(m.get_editor_property('anim_class')))
s = m.get_editor_property('skeletal_mesh_asset')
unreal.log('HAND_INSPECT skeleton=' + str(s.get_editor_property('skeleton')) + ' post=' + str(s.get_editor_property('post_process_anim_blueprint')))
