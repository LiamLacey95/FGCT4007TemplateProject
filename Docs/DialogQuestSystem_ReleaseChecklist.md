# DialogQuestSystem Release Checklist

## Editor UX

- Guided Mode hides advanced palette items and keeps guidance visible
- Full Editor exposes the broader node set
- starter graphs open with helper comments
- guide panel toggles as a real docked tab
- validation panel shows grouped issues and fix/jump actions
- simulation labels match current node semantics

## Runtime UX

- default dialogue widget wraps and scrolls long text safely
- large choice sets remain readable
- quest tracker stays compact
- quest log handles long titles and descriptions
- cursor and player input handoff works during dialogue

## Sample content

- sample dialogue asset exists under plugin content
- sample quest asset exists under plugin content
- sample dialogue references the sample quest through a quest hook

## Packaging

- plugin packages with `BuildPlugin`
- packaged output loads in another UE5 project
- editor module stays editor-only
- sample assets load from plugin content in the target project

## Documentation

- local docs index is current
- docs-site builds with MkDocs
- screenshot folder structure exists
- GitHub Pages workflow is present
