# Community template compatibility

MationBuild is designed to support community-authored decoration packs, including templates shared by Antiprime's AutoBuild community where the creator permits redistribution or use. No third-party template files are bundled by default.

## What compatibility means today

The current mod build is an early entry-point scaffold; it does **not yet parse or apply Antiprime's native template files**. Before native compatibility can be implemented safely, we need a representative template export and its format/version. Do not assume a template ID, website URL, or level ID alone contains enough data to import a template.

## Proposed portable manifest

Community submissions should provide metadata and point to the actual template payload. The service currently stores metadata for moderation, not the payload itself. A future importer should validate the payload schema and enforce object-count limits before changing a level.

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