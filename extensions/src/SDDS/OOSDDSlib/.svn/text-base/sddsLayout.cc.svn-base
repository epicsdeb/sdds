
#include "sddsLayout.hh"
#include "tsDLList.h"

sddsLayout::sddsLayout(sddsError &errorStack) 
  : err(errorStack) 
{

  printf("sddsLayout: construct\n");
  deferSavingLayout = 0;
  n_columns = 0;
  n_parameters = 0;
  n_associates = 0;
  n_arrays = 0;

  description.set(NULL);
  contents.set(NULL);
  version = 0;
  layout_written = 0;
  // data_mode initialized by its constructor
  column_definition = NULL;
  parameter_definition = NULL;
  array_definition = NULL;
  associate_definition = NULL;
  column_index = NULL;
  parameter_index = NULL;
  array_index = NULL;
}

void
sddsLayout::commitParameterDefinition(sddsParameterDefinition *parameterDef) {
  printf("parameter %s parsed-->\n",parameterDef->getName());
  //  parameter_definition_table.add(parameterDef->getName(),*parameterDef);
}

long
sddsLayout::SDDS_ReadLayout(FILE *fp) {
  FILE *fp1;
  long retval;
  static long depth = 0;
  static long data_command_seen = 0;

  if (!fp) {
    err.SDDS_SetError("Unable to read layout--NULL file pointer (SDDS_ReadLayout)");
    return(0);
  }

  layout_written = 1;
  if (headerParse(fp, this)) {
    return(0);
  }
  return(1);
  /*
  if (depth==0)
    data_command_seen = 0;
  while (get_namelist(s, SDDS_MAXLINE, fp)) {
    scan_namelist(&namelist_text, s);
    switch (match_string(namelist_text.group_name, SDDS_command, SDDS_NUM_COMMANDS, EXACT_MATCH)) {
    case SDDS_DESCRIPTION_COMMAND:
      if (!SDDS_ProcessDescription(SDDS_dataset, &namelist_text)) {
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to process description (SDDS_ReadLayout)");
        return(0);
      }
      break;
    case SDDS_COLUMN_COMMAND:
      if (!SDDS_ProcessColumnDefinition(SDDS_dataset, &namelist_text)) {
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to process column definition (SDDS_ReadLayout)");
        return(0);
      }
      break;
    case SDDS_PARAMETER_COMMAND:
      if (!SDDS_ProcessParameterDefinition(SDDS_dataset, &namelist_text)) {
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to process parameter definition (SDDS_ReadLayout)");
        return(0);
      }
      break;
    case SDDS_ASSOCIATE_COMMAND:
#if defined(RW_ASSOCIATES)
      if (!SDDS_ProcessAssociateDefinition(SDDS_dataset, &namelist_text)) {
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to process associate definition (SDDS_ReadLayout)");
        return(0);
      }
#endif
      break;
    case SDDS_DATA_COMMAND:
      if (!SDDS_ProcessDataMode(SDDS_dataset, &namelist_text)) {
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to process data mode (SDDS_ReadLayout)");
        return(0);
      }
      if (data_command_seen) {
        //should never happen
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to read layout--multiple data commands (SDDS_ReadLayout)");
        return(0);
      }
      if (!SDDS_SaveLayout(SDDS_dataset)) {
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to read layout--couldn't save layout (SDDS_ReadLayout)");
        return(0);
      }
      data_command_seen = 1;
      free_namelist_text(&namelist_text);
      return(1);
      break;
    case SDDS_INCLUDE_COMMAND:
      if (!(fp1=SDDS_ProcessIncludeCommand(SDDS_dataset, &namelist_text))) {
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to process include command (SDDS_ReadLayout)");
        return(0);
      }
      depth += 1;
      retval = SDDS_ReadLayout(SDDS_dataset, fp1);
      depth -= 1;
      fclose(fp1);
      if (retval==0) {
        free_namelist_text(&namelist_text);
        return(0);
      }
      if (data_command_seen) {
        free_namelist_text(&namelist_text);
        return(1);
      }
      break;
    case SDDS_ARRAY_COMMAND:
      if (!SDDS_ProcessArrayDefinition(SDDS_dataset, &namelist_text)) {
        fclose(fp);
        free_namelist_text(&namelist_text);
        SDDS_SetError("Unable to process array definition (SDDS_ReadLayout)");
        return(0);
      }
      break;
    default:
      fclose(fp);
      free_namelist_text(&namelist_text);
      sprintf(s, "Unknown layout entry %s given (SDDS_ReadLayout)", namelist_text.group_name);
      SDDS_SetError(s); 
      return(0);
      break;
    }
    free_namelist_text(&namelist_text);
  }
  // on recursive calls, it's okay to hit EOF
  if ((feof(fp) && depth!=0) || data_command_seen)
    return(1);
    */
  return(0);


}

