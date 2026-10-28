# Community template compatibility

MationBuild is designed to support community-authored decoration packs, including templates shared by Antiprime's AutoBuild community where the creator permits redistribution or use. No third-party template files are bundled by default.

## What compatibility means today

The current mod has a first selection-based accent pass, but it does **not yet parse or apply Antiprime's native template files**. Before native compatibility can be implemented safely, we need a representative template export and its format/version. Do not assume a template ID, website URL, or level ID alone contains enough data to import a template.

## Portable template format (implemented)

Community submissions should provide metadata and point to a downloadable JSON payload. The service stores metadata for moderation, not the payload itself. In Geometry Dash's editor, use the **Import** button and select the downloaded JSON file; select one or more anchor objects first. Each template object's x/y values are offsets from each selected object's position.

The importer accepts `mationbuild-json`, schema version 1, for Geometry Dash `2.2081`. To keep imports safe, the current format only allows the five tiny decorative accent IDs 1764–1768, caps templates at 100 objects, caps files at 500 KB, limits anchors and total placements, and validates coordinate/scale/rotation ranges. It does not allow arbitrary triggers or gameplay objects.

Example payload: [download the sample Tech accent pack](https://raw.githubusercontent.com/SolanceGMD/MationBuild/main/templates/tech-accent-pack.json).
```json
{
  "format": "mationbuild-json",
  "schema_version": 1,
  "game_version": "2.2081",
  "objects": [
    { "id": 1764, "x": -10, "y": 10, "scale": 0.35, "rotation": 0 },
    { "id": 1765, "x": 10, "y": -10, "scale": 0.35, "rotation": 0 },
    { "id": 1766, "x": -10, "y": -10, "scale": 0.35, "rotation": 0 },
    { "id": 1767, "x": 10, "y": 10, "scale": 0.35, "rotation": 0 }
  ]
}
```

The community catalog currently stores metadata and HTTPS source/download links. Moderators should verify that the payload follows this schema and that the submitter has permission to share it before publishing.

Example manifest:
```json
{
  "schema_version": 1,
  "slug": "neon-edge-kit",
  "title": "Neon Edge Kit",
  "author": "CreatorName",
  "description": "Neon edge decorations for block structures.",
  "license": "CC-BY-4.0",
  "source_url": "https://example.com/original-template",
  "game_version": "2.2074",
  "template_format": "mationbuild-json",
  "payload_url": "https://example.com/neon-edge-kit.json",
  "tags": ["neon", "modern", "edges"]
}
```

## Import and safety policy

- Credit the original author and preserve any license and attribution requirements.
- Never scrape private downloads, bypass key checks, or redistribute paid packs without permission.
- Native Antiprime compatibility requires an authorized sample/export and format-specific parser; this scaffold does not claim that compatibility already works.
- Treat downloaded template payloads as untrusted data, not executable code. Validate schema, size, object IDs, coordinates, and version compatibility before applying.
- Keep a preview and an undo path before applying a template to a level.
- Community submissions remain pending until a moderator approves them.