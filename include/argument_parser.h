#pragma once
#include <unordered_map>
#include <iostream>
#include <string>

// Constants used to print in color to the command line
// Taken from https://stackoverflow.com/a/9158263
#define RESET       "\033[0m"
#define BLACK       "\033[30m"             // Black
#define RED         "\033[31m"             // Red
#define GREEN       "\033[32m"             // Green
#define YELLOW      "\033[33m"             // Yellow
#define BLUE        "\033[34m"             // Blue
#define MAGENTA     "\033[35m"             // Magenta
#define CYAN        "\033[36m"             // Cyan
#define WHITE       "\033[37m"             // White
#define BOLDBLACK   "\033[1m\033[30m"      // Bold Black
#define BOLDRED     "\033[1m\033[31m"      // Bold Red
#define BOLDGREEN   "\033[1m\033[32m"      // Bold Green
#define BOLDYELLOW  "\033[1m\033[33m"      // Bold Yellow
#define BOLDBLUE    "\033[1m\033[34m"      // Bold Blue
#define BOLDMAGENTA "\033[1m\033[35m"      // Bold Magenta
#define BOLDCYAN    "\033[1m\033[36m"      // Bold Cyan
#define BOLDWHITE   "\033[1m\033[37m"      // Bold White

/**
 * @brief This class is used to define rules for command line arguments to the 
 * executable that this ArgumentParser is associated with. Add rules for command
 * line arguments using the `add_argument` function, then call the `parse` 
 * function to fill the variables pointed at in the rule definitions either with
 * the arguments pass in the command line or with the default values of the type
 * if the arguments were not specified.
 */
class ArgumentParser {
  public:
    /**
     * @brief Construct a new Argument Parser object.
     */
    ArgumentParser() : args_() { }

    /**
     * @brief Add a rule for an integer argument in the executable that this
     * ArgumentParser is linked to. The argument will default to `0` unless the 
     * argument is passed in via the command line.
     * 
     * @param flag a string indicating the flag used to declare the command line 
     * argument. 
     * @param required a bool indicating whether this command line argument is 
     * required in the context of the executable.
     * @param var an int* that points to a variable that will be filled with the
     * value passed in the command line, if the argument is passed. 
     * @param description a string containing the details on the usage and 
     * purpose for this argument.
     */
    void add_argument(const std::string& flag, bool required, int* var, 
                      const std::string& description="") {
      *var = 0;     
      args_[flag] = ArgumentConfig(flag, INT, required, var, description);
    }

    /**
     * @brief Add a rule for a boolean argument in the executable that this
     * ArgumentParser is linked to. The argument will default to `false` unless 
     * the argument is passed in via the command line.
     * 
     * @param flag a string indicating the flag used to declare the command line 
     * argument. 
     * @param required a bool indicating whether this command line argument is 
     * required in the context of the executable.
     * @param var a bool* that points to a variable that will be filled with the
     * value passed in the command line, if the argument is passed. 
     * @param description a string containing the details on the usage and 
     * purpose for this argument.
     */
    void add_argument(const std::string& flag, bool required, bool* var, 
                      const std::string& description="") {
      *var = false;
      args_[flag] = ArgumentConfig(flag, BOOL, required, var, description);
    }

    /**
     * @brief Add a rule for a string argument in the executable that this
     * ArgumentParser is linked to. The argument will default to an empty string
     * unless the argument is passed in via the command line.
     * 
     * @param flag a string indicating the flag used to declare the command line 
     * argument. 
     * @param required a bool indicating whether this command line argument is 
     * required in the context of the executable.
     * @param var a string* that points to a variable that will be filled with 
     * the value passed in the command line, if the argument is passed. 
     * @param description a string containing the details on the usage and 
     * purpose for this argument.
     */
    void add_argument(const std::string& flag, bool required, std::string* var, 
                      const std::string& description="") {
      *var = "";
      args_[flag] = ArgumentConfig(flag, STRING, required, var, description);
    }

    /**
     * @brief Add a rule for a double argument in the executable that this
     * ArgumentParser is linked to. The argument will default to `0.0` unless 
     * the argument is passed in via the command line.
     * 
     * @param flag a string indicating the flag used to declare the command line 
     * argument. 
     * @param required a bool indicating whether this command line argument is 
     * required in the context of the executable.
     * @param var a double* that points to a variable that will be filled with 
     * the value passed in the command line, if the argument is passed. 
     * @param description a string containing the details on the usage and 
     * purpose for this argument.
     */
    void add_argument(const std::string& flag, bool required, double* var, 
                      const std::string& description="") {
      *var = 0.0;
      args_[flag] = ArgumentConfig(flag, DOUBLE, required, var, description);
    }
    
    /**
     * @brief Parse all of the command line arguments and fill all of the 
     * variables each argument rule points at with the values specified in the 
     * command line, if they were specified. This function will return an error
     * code if either at least one required command line argument was not 
     * specified or if at least one command line argument passed was invalid
     * according to the rule with the matching command line argument flag. If 
     * all of the arguments could be processed and all required arguments were
     * specified, this function returns a success status of 0.
     * 
     * @param arg_count an int indicating the number of arguments included in 
     * the command used to run the executable this ArgumentParser is linked to. 
     * The `argc` parameter in a `main` function in a `.cpp` file should be 
     * passed direcly into this parameter.
     * @param arg_values an array of char* containing each arguments included in 
     * the command used to run the executable this ArgumentParser is linked to. 
     * The `argv` parameter in a `main` function in a `.cpp` file should be 
     * passed directly into this parameter. 
     * @return 0 if all required command line arguments were provided. 
     * @return 1 if a required command line argument was not provided.
     */
    int parse(int arg_count, char* arg_values[]) {
      // load all of the arguments passed via the command line
      for (int i = 1; i < arg_count; ++i) {
        // skip the executable name (the 0th argument) by starting at 1
        std::string flag = arg_values[i];
        auto iter = args_.find(flag);

        if (iter == args_.end()) {
          std::cout << BOLDRED << "Unknown flag: " << flag << RESET << std::endl;
          continue;
        } 

        // handle case where flag is provided but no value is given (except bools)
        if (iter->second.data_type_ != BOOL) {
          if (i + 1 == arg_count || args_.find(arg_values[i + 1]) != args_.end()) {
            // avoid segfault if the value to the last argument is not provided
            handleInvalidArgument(flag);
            continue;
          }
        }

        void* to_fill = args_[flag].variable_to_fill_;
        args_[flag].filled_ = true;
        
        // if current argument in iteration is valid, parse and save value
        if (iter->second.data_type_ == INT) {
          try {
            *static_cast<int*>(to_fill) = std::stoi(arg_values[++i]);
          } catch (const std::invalid_argument& e) {
            // could not parse integer argument, so fail to parse
            handleInvalidArgument(flag);
            continue;
          }
        } else if (iter->second.data_type_ == BOOL) {
          *static_cast<bool*>(to_fill) = true;
        } else if (iter->second.data_type_ == DOUBLE) {
          try {
            *static_cast<double*>(to_fill) = std::stod(arg_values[++i]);
          } catch (const std::invalid_argument& e) {
            // could not parse integer argument, so fail to parse
            handleInvalidArgument(flag);
            continue;
          }
        } else if (iter->second.data_type_ == STRING) {
          *static_cast<std::string*>(to_fill) = arg_values[++i];
        }
      }

      // check if any required arguments have not been provided
      bool all_required_args_provided = true;
      for (std::pair<std::string, ArgumentConfig> p : args_) {
        if (p.second.required_ && !p.second.filled_) {
          std::cout << BOLDRED << "Missing required argument " << p.first
                    << ": [" << typeAsString(p.second.data_type_) << "] "
                    << p.second.description_ << RESET << std::endl;
          all_required_args_provided = false;
        }
      }

      return all_required_args_provided && !has_invalid_arguments_ ? 0 : 1;
    }
  private:
    /**
     * @brief This enumeration is used internally to keep track of the 
     * underlying type of each command line argument's representative variable.
     * This allows us to use a single data structure to keep track of all 
     * arguments with varying underlying types. This information enables us to 
     * store the representative variables as void* internally and type cast to
     * a pointer to the specific variable whenever required. 
     */
    enum DataType {
      INT,
      BOOL,
      DOUBLE,
      STRING
    };
    
    /**
     * @brief This struct is used to model the command line arguments that a 
     * program can expect to see when its associated executable is run on a 
     * command line interface. Boolean command line arguments are set to true or
     * false by simply adding/omitting the flag alone. All other types of 
     * command line arguments must follow the flag with the value to pass in as 
     * an argument. If the flag is not specified, the default value is assumed.
     */
    struct ArgumentConfig {
      /**
       * @brief A string indicating the flag used to declare the command line 
       * argument. 
       */
      std::string flag_;

      /**
       * @brief A string containing the description of the use of this command 
       * line argument in the context of the executable. 
       */
      std::string description_;

      /**
       * @brief A DataType enum indicating the type this argument must take. 
       */
      DataType data_type_;

      /**
       * @brief A bool indicating whether this command line argument is required
       * in the context of the executable. The parser will produce a warning if 
       * there is a required variable that has not been provided in the command
       * line. 
       */
      bool required_;

      /**
       * @brief A bool indicating whether this argument was defined in the 
       * arguments passed in the command line. Unfilled required arguments will
       * trigger a warning displayed in the command line.
       */
      bool filled_;

      /**
       * @brief A void* that points to the variable to populate with the value
       * passed in via the command line. If an argument is not provided in the 
       * command line, this variable takes on the default value of the type. 
       * This void* must be cast to the type of the variable pointed to in order
       * to write the appropriate value. The proper typecasting is completed by 
       * referring to the data_type_ field and casting as appropriate.
       */
      void* variable_to_fill_;

      /**
       * @brief Construct a default ArgumentConfig object.
       */
      ArgumentConfig() : flag_(), description_(), data_type_(), required_(false),
                          filled_(false), variable_to_fill_() { }

      /**
       * @brief Construct a new Argument Config object with the given parameters.
       * 
       * @param flag a string indicating the flag used to declare the command 
       * line argument. 
       * @param data_type a DataType enumeration indicating the type that the 
       * argument this struct represents must take. 
       * @param required a bool indicating whether this command line argument is
       * required in the context of the executable. 
       * @param var a void* that points to the variable to populate with the 
       * value passed in via the command line. 
       * @param description a string containing the description of the use of 
       * this command line argument in the context of the executable. 
       */
      ArgumentConfig(const std::string& flag, DataType data_type, bool required,
                     void* var, const std::string& description)
        : flag_(flag), description_(description), data_type_(data_type),
          required_(required), filled_(false), variable_to_fill_(var) { }
    };

    /**
     * @brief This structure associates each command line argument rule with the
     * flag used to define the value of an argument in the command line.
     */
    std::unordered_map<std::string, ArgumentConfig> args_;

    /**
     * @brief A bool indicating whether or not at least one of the arguments 
     * that were passed in via the command line and parsed by this class is 
     * invalid.
     */
    bool has_invalid_arguments_;

    /**
     * @brief Map a DataType enum variant to its typed string representation.
     * 
     * @param t a DataType enum to convert to a string.
     * @return a std::string representing the text form of the passed enum.
     */
    std::string typeAsString(DataType t) const {
      if (t == INT)      return "INTEGER";
      if (t == BOOL)     return "BOOLEAN";
      if (t == DOUBLE)   return "DOUBLE";
      if (t == STRING)   return "STRING";

      return "";   
    }

    /**
     * @brief Helper function that prints out a message indicating that the 
     * argument given for the passed flag was invalid. Marks the interval 
     * `has_invalid_arguments_` state as true for the `parse` function to use.
     * 
     * @param flag a string indicating the flag that received an invalid 
     * argument.
     */
    void handleInvalidArgument(const std::string& flag) {
      std::cout << BOLDRED << "Invalid argument " << flag << ": [" 
                << typeAsString(args_[flag].data_type_) << "] "
                << args_[flag].description_ << RESET << std::endl;

      has_invalid_arguments_ = true;
    }
};
