// Glaze Library
// For the license information refer to glaze.hpp

#pragma once

// TODO: this only handles /**/ style comments

namespace glaze
{
   namespace detail
   {
      enum class general_state : uint32_t {
         NORMAL,
         ESCAPED,
         STRING,
         BEFORE_ASTERISK,
         COMMENT,
         BEFORE_FSLASH
      };

      inline void prettify_normal_state(const char c, auto& out, int& indent, auto nl,
                                 general_state& state) noexcept
      {
         switch (c) {
         case ',':
            out += c;
            nl();
            break;
         case '[':
            out += c;
            ++indent;
            nl();
            break;
         case ']':
            --indent;
            nl();
            out += c;
            break;
         case '{':
            out += c;
            ++indent;
            nl();
            break;
         case '}':
            --indent;
            nl();
            out += c;
            break;
         case '\\':
            out += c;
            state = general_state::ESCAPED;
            break;
         case '\"':
            out += c;
            state = general_state::STRING;
            break;
         case '/':
            out += " /";
            state = general_state::BEFORE_ASTERISK;
            break;
         case ':':
            out += ": ";
            break;
         case ' ':
         case '\n':
         case '\r':
         case '\t':
            break;
         default:
            out += c;
            break;
         }
      }

      inline void prettify_other_states(const char c, general_state& state) noexcept
      {
         switch (state) {
         case general_state::ESCAPED:
            state = general_state::NORMAL;
            break;
         case general_state::STRING:
            if (c == '"') {
               state = general_state::NORMAL;
            }
            break;
         case general_state::BEFORE_ASTERISK:
            state = general_state::COMMENT;
            break;
         case general_state::COMMENT:
            if (c == '*') {
               state = general_state::BEFORE_FSLASH;
            }
            break;
         case general_state::BEFORE_FSLASH:
            if (c == '/') {
               state = general_state::NORMAL;
            }
            else {
               state = general_state::COMMENT;
            }
            break;
         default:
            break;
         }
      }
   }

   /// <summary>
   /// pretty print a JSON string
   /// </summary>
   inline void prettify(const auto& in, auto& out, const bool tabs = false,
                 const int indent_size = 3) noexcept
   {
      out.reserve(in.size());
      int indent{};

      auto nl = [&]() {
         out += "\n";

         for (auto i = 0; i < indent * (tabs ? 1 : indent_size); i++) {
            out += tabs ? "\t" : " ";
         }
      };
      
      using namespace detail;
      general_state state{general_state::NORMAL};

      for (auto c : in) {
         if (state == general_state::NORMAL) {
            prettify_normal_state(c, out, indent, nl, state);
            continue;
         }
         else {
            out += c;
            prettify_other_states(c, state);
         }
      }
   }
   
   /// <summary>
   /// allocating version of prettify
   /// </summary>
   inline std::string prettify(const auto& in, const bool tabs = false,
                 const int indent_size = 3) noexcept
   {
      std::string out{};
      prettify(in, out, tabs, indent_size);
      return out;
   }
}
