#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/codegen.h"

// Generate Python dataclass for a C struct
void generate_structs(FILE *fp, Struct *structs, int struct_count)
{
    if (struct_count == 0)
        return;

    fprintf(fp, "# Struct definitions\n");
    for (int i = 0; i < struct_count; i++)
    {
        Struct *s = &structs[i];

        fprintf(fp, "@dataclass\n");
        fprintf(fp, "class %s:\n", s->name);

        // Generate fields with type hints
        for (int j = 0; j < s->field_count; j++)
        {
            Variable *field = &s->fields[j];
            indent(fp, 1);
            if (field->is_array)
            {
                fprintf(fp, "%s: List[%s] = field(default_factory=lambda: [0] * %d)\n",
                        field->name, get_python_type_name(field->type), field->array_size);
            }
            else
            {
                const char *default_value = field->type == TYPE_INT ? "0" : 
                                          field->type == TYPE_FLOAT ? "0.0" : 
                                          field->type == TYPE_CHAR ? "''" : 
                                          "None";
                fprintf(fp, "%s: %s = %s\n", field->name, 
                        field->struct_name ? field->struct_name : get_python_type_name(field->type), 
                        default_value);
            }
        }
        fprintf(fp, "\n");
    }
}