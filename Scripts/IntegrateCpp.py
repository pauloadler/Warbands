"""Run with UnrealEditor-Cmd -run=pythonscript -script=... after building WarbandsEditor."""
import unreal

ROOT = "/Game/Warbands"
assets = unreal.EditorAssetLibrary
editor = unreal.BlueprintEditorLibrary

widget = assets.load_asset(ROOT + "/UI/WBP_CharacterCreation")
assert widget, "Missing character creation widget"
preparation = unreal.WarbandsEditorBridge.prepare_character_creation(widget)
unreal.log(preparation)
assert preparation.startswith("OK"), preparation
editor.reparent_blueprint(widget, unreal.WarbandsCharacterCreation)
editor.compile_blueprint(widget)
assert assets.save_loaded_asset(widget), "Could not save widget"

controller_path = ROOT + "/Blueprints/BP_WarbandsPlayerController"
if not assets.does_asset_exist(controller_path):
    controller = assets.duplicate_asset(
        "/Game/ThirdPerson/Blueprints/BP_ThirdPersonPlayerController", controller_path)
else:
    controller = assets.load_asset(controller_path)
assert controller
editor.reparent_blueprint(controller, unreal.WarbandsPlayerController)
editor.compile_blueprint(controller)
controller_class = assets.load_blueprint_class(controller_path)
defaults = unreal.get_default_object(controller_class)
defaults.set_editor_property("character_creation_class", assets.load_blueprint_class(ROOT + "/UI/WBP_CharacterCreation"))
defaults.set_editor_property("sword_class", assets.load_blueprint_class(ROOT + "/Weapons/Sword/BP_Sword"))
defaults.set_editor_property("sword_grip_offset", unreal.Transform(location=unreal.Vector(-39, 0, 0), scale=unreal.Vector(0.5, 0.5, 0.5)))
assert assets.save_loaded_asset(controller)

mode = assets.load_asset(ROOT + "/Blueprints/BP_WarbandsGameMode")
mode_class = assets.load_blueprint_class(ROOT + "/Blueprints/BP_WarbandsGameMode")
mode_defaults = unreal.get_default_object(mode_class)
mode_defaults.set_editor_property("player_controller_class", controller_class)
mode_defaults.set_editor_property("default_pawn_class", assets.load_blueprint_class(ROOT + "/Blueprints/Characters/BP_PlayerCharacter"))
assert assets.save_loaded_asset(mode)

level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
assert level.load_level(ROOT + "/Maps/Lvl_Warbands")
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property("default_game_mode", mode_class)
for actor in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors():
    if actor.get_class().get_name() == "BP_Sword_C":
        for component in actor.get_components_by_class(unreal.StaticMeshComponent):
            mesh = component.get_editor_property("static_mesh")
            unreal.log("SWORD_PART " + str(component.get_name()) + " transform=" + str(component.get_relative_transform()) + " bounds=" + str(mesh.get_bounding_box()))
assert level.save_current_level()
unreal.log("WARBANDS_CPP_INTEGRATION_OK")
