#pragma once
#include <stdexcept>
#include <string>
#include <string_view>
#include <optional>
#include <utility>

class MatrixException : public std::runtime_error {
public:
    enum class ErrorType {
        AllocationError,
        EmptyMatrix,
        SizeMismatch,
        InvalidData,
        OutOfBounds,
        InvalidArgument
    };

    MatrixException(std::string_view message, ErrorType type)
        : std::runtime_error(std::string(message)), type_(type) {}

    MatrixException(std::string_view message, ErrorType type,
                    std::pair<std::size_t, std::size_t> dimensions)
        : std::runtime_error(std::string(message)), type_(type), dimensions_(dimensions) {}

    MatrixException(std::string_view message, ErrorType type,
                    std::pair<std::size_t, std::size_t> dimensions1,
                    std::pair<std::size_t, std::size_t> dimensions2)
        : std::runtime_error(std::string(message)), type_(type),
          dimensions_(dimensions1), other_dimensions_(dimensions2) {}

    MatrixException(std::string_view message, ErrorType type,
                    std::size_t row, std::size_t col)
        : std::runtime_error(std::string(message)), type_(type), index_(std::make_pair(row, col)) {}

    ErrorType type() const noexcept { return type_; }
    std::optional<std::pair<std::size_t, std::size_t>> dimensions() const noexcept {
        return dimensions_;
    }
    std::optional<std::pair<std::size_t, std::size_t>> other_dimensions() const noexcept {
        return other_dimensions_;
    }
    std::optional<std::pair<std::size_t, std::size_t>> index() const noexcept {
        return index_;
    }

private:
    ErrorType type_;
    std::optional<std::pair<std::size_t, std::size_t>> dimensions_;
    std::optional<std::pair<std::size_t, std::size_t>> other_dimensions_;
    std::optional<std::pair<std::size_t, std::size_t>> index_;
};
