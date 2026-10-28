# Community template compatibility

MationBuild supports portable, community-authored decoration packs in its documented JSON format. Use only packs you have permission to use, and preserve author credit and license requirements.

## Compatibility today

The in-editor importer supports **MationBuild JSON v1** for Geometry Dash **2.2081**. It does **not** currently parse native Antiprime AutoBuild files. That format must be verified from an authorized sample/export before native compatibility can be implemented safely. Do not assume a level ID, template name, or website URL is itself an importable template.

## Portable format

Select one or more anchor objects in the editor, press **Import**, and choose a `.json` file. Each object's `x` and `y` values are offsets from every selected anchor. The sample [Tech Panel Frame pack](https://raw.githubusercontent.com/SolanceGMD/MationBuild/main/templates/tech-accent-pack.json) demonstrates layered line frames, corner markers, and a center detail.

```json
{
  "format": "mationbuild-json",
  "schema_version": 1,
  "game_version": "2.2081",
  "objects": [
    { "id": 1754, "x": 0, "y": 15, "scale": 0.62, "rotation": 0 },
    { "id": 1753, "x": 0, "y": -15, "scale": 0.62, "rotation": 0 },
    { "id": 1764, "x": -12, "y": 12, "scale": 0.32, "rotation": 0 },
    { "id": 1766, "x": 0, "y": 0, "scale": 0.3, "rotation": 90 }
  ]
}
```

## Validation and limits

- Files must be valid JSON and no larger than 500 KB.
- A pack may contain 1–100 objects.
- Only an explicit allowlist of decorative object IDs is accepted; gameplay objects and triggers are excluded.
- Coordinates, scale, rotation, selected anchor count, and total placements are range-checked.
- Use Undo to revert a generated pass, and test in a copy of important levels first.

## Community publishing

The catalog stores template metadata and source/download links. Moderators should validate the payload and confirm redistribution permission before publishing. Preserve the original creator's attribution and license.

MationBuild is independent and is not affiliated with RobTop Games or Antiprime.
