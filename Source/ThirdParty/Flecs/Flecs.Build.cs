using System.IO;
using UnrealBuildTool;
 
public class Flecs: ModuleRules {
    public Flecs(ReadOnlyTargetRules Target) : base(Target) {
        Type = ModuleType.External;
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "flecs", "src"));
    }
}