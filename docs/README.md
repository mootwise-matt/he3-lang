# He³ Documentation

This directory contains the Jekyll-based documentation for the He³ (Helium-3) Programming Language.

## Local Development

To run the documentation site locally:

```bash
cd docs
bundle install
bundle exec jekyll serve
```

The site will be available at `http://localhost:4000`.

## GitHub Pages

This documentation is automatically deployed to GitHub Pages when changes are pushed to the main branch.

## Structure

- `_config.yml` - Jekyll configuration
- `_layouts/` - HTML templates
- `_language/` - Language documentation
- `_compiler/` - Compiler documentation  
- `_vm/` - Virtual Machine documentation
- `_bytecode/` - Bytecode documentation
- `_exporter/` - Exporter documentation
- `_development/` - Development documentation
- `index.md` - Homepage