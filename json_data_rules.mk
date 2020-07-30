# JSON files are run through jsonproc, which is a tool that converts JSON data to an output file
# based on an Inja template. https://github.com/pantor/inja

AUTO_GEN_TARGETS += $(DATA_SRC_SUBDIR)/wild_encounters.h


$(DATA_SRC_SUBDIR)/wild_encounters.h: $(DATA_SRC_SUBDIR)/wild_encounters.json $(DATA_SRC_SUBDIR)/wild_encounters.json.txt
	$(JSONPROC) $^ $@

$(C_BUILDDIR)/wild_encounter.o: c_dep += $(DATA_SRC_SUBDIR)/wild_encounters.h



SPECIES_JSON := $(DATA_SRC_SUBDIR)/pokemon_species.json
TEXT_SUBDIR := $(DATA_SRC_SUBDIR)/text
PKMN_SUBDIR := $(DATA_SRC_SUBDIR)/pokemon
PKMN_GRAPHICS_SUBDIR := $(DATA_SRC_SUBDIR)/pokemon_graphics


SPECIES_TEMPLATES := include/constants/species.h 	\
	$(TEXT_SUBDIR)/species_names.h 					\
	$(PKMN_SUBDIR)/pokedex_text.h					\
	$(PKMN_SUBDIR)/level_up_learnset_pointers.h 	\
	$(PKMN_SUBDIR)/tmhm_learnsets.h 				\
	$(PKMN_SUBDIR)/tutor_learnsets.h 				\
	$(PKMN_SUBDIR)/egg_moves.h						\
	$(PKMN_SUBDIR)/base_stats.h						\
	$(PKMN_SUBDIR)/evolution.h						\
	$(PKMN_GRAPHICS_SUBDIR)/front_pic_coordinates.h	\
	$(PKMN_GRAPHICS_SUBDIR)/back_pic_coordinates.h	\
	$(PKMN_GRAPHICS_SUBDIR)/footprint_table.h		\
	$(PKMN_GRAPHICS_SUBDIR)/icon.h					\
	$(PKMN_GRAPHICS_SUBDIR)/jump.h					\
	$(PKMN_GRAPHICS_SUBDIR)/animation.h

SPECIES_TEMPLATES_TEST := $(addsuffix .tmp.h, $(SPECIES_TEMPLATES))

AUTO_GEN_TARGETS += $(SPECIES_TEMPLATES_TEST)


$(SPECIES_TEMPLATES_TEST): %.h.tmp.h: $(SPECIES_JSON) %.h.template
	$(JSONPROC) $^ $@

species_templates: $(SPECIES_TEMPLATES_TEST)
