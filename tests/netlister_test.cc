#include "netlister.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "proto/spice_simulator.pb.h"
#include "vlsir/spice.pb.h"

namespace spiceserver {
namespace {

class NetlisterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a temporary directory for test outputs
    test_dir_ = std::filesystem::temp_directory_path() /
                ("netlister_test_" + std::to_string(getpid()));
    std::filesystem::create_directories(test_dir_);

    netlister_ = &Netlister::GetInstance();
  }

  void TearDown() override {
    // Clean up the temporary directory
    if (std::filesystem::exists(test_dir_)) {
      std::filesystem::remove_all(test_dir_);
    }
  }

  Netlister *netlister_;
  std::filesystem::path test_dir_;
};

TEST_F(NetlisterTest, WritesProtobufToFile) {
  // Create a simple SimInput protobuf
  vlsir::spice::SimInput sim_input;
  sim_input.set_top("test_top");

  // Add a simple package
  auto* pkg = sim_input.mutable_pkg();
  pkg->set_domain("test_domain");

  // Call WriteSim
  Flavour flavour = Flavour::XYCE;
  auto result = netlister_->WriteSim(sim_input, flavour, test_dir_);

  // Verify the output file was created
  std::filesystem::path expected_file = test_dir_ / "sim_input.pb";
  ASSERT_TRUE(std::filesystem::exists(expected_file))
      << "Expected file " << expected_file << " does not exist";

  // Verify we can read the file back
  std::fstream input_file(expected_file.string(),
                          std::ios::in | std::ios::binary);
  ASSERT_TRUE(input_file.is_open())
      << "Could not open file " << expected_file;

  // Parse the protobuf from the file
  vlsir::spice::SimInput read_sim_input;
  ASSERT_TRUE(read_sim_input.ParseFromIstream(&input_file))
      << "Failed to parse protobuf from file";

  // Verify the contents match what we wrote
  EXPECT_EQ(read_sim_input.top(), "test_top");
  EXPECT_EQ(read_sim_input.pkg().domain(), "test_domain");
}

TEST_F(NetlisterTest, HandlesEmptySimInput) {
  // Create an empty SimInput protobuf
  vlsir::spice::SimInput sim_input;

  // Call WriteSim with empty input
  Flavour flavour = Flavour::NGSPICE;
  auto result = netlister_->WriteSim(sim_input, flavour, test_dir_);

  // Verify the output file was still created
  std::filesystem::path expected_file = test_dir_ / "sim_input.pb";
  ASSERT_TRUE(std::filesystem::exists(expected_file));

  // Verify we can read it back
  std::fstream input_file(expected_file.string(),
                          std::ios::in | std::ios::binary);
  vlsir::spice::SimInput read_sim_input;
  ASSERT_TRUE(read_sim_input.ParseFromIstream(&input_file));
}

TEST_F(NetlisterTest, WorksWithDifferentFlavours) {
  vlsir::spice::SimInput sim_input;
  sim_input.set_top("flavour_test");

  // Test with different flavours
  std::vector<Flavour> flavours = {
    Flavour::XYCE,
    Flavour::NGSPICE,
    Flavour::SPECTRE,
    Flavour::HSPICE
  };

  for (auto flavour : flavours) {
    // Create a subdirectory for each flavour
    auto flavour_dir = test_dir_ / std::to_string(static_cast<int>(flavour));
    std::filesystem::create_directories(flavour_dir);

    auto result = netlister_->WriteSim(sim_input, flavour, flavour_dir);

    // Verify file was created for each flavour
    std::filesystem::path expected_file = flavour_dir / "sim_input.pb";
    EXPECT_TRUE(std::filesystem::exists(expected_file))
        << "File not created for flavour " << static_cast<int>(flavour);
  }
}

TEST_F(NetlisterTest, OverwritesExistingFile) {
  vlsir::spice::SimInput first_input;
  first_input.set_top("first");

  vlsir::spice::SimInput second_input;
  second_input.set_top("second");

  Flavour flavour = Flavour::XYCE;
  std::filesystem::path expected_file = test_dir_ / "sim_input.pb";

  // Write first time
  netlister_->WriteSim(first_input, flavour, test_dir_);
  ASSERT_TRUE(std::filesystem::exists(expected_file));

  // Write second time (should overwrite)
  netlister_->WriteSim(second_input, flavour, test_dir_);

  // Verify the file contains the second input
  std::fstream input_file(expected_file.string(),
                          std::ios::in | std::ios::binary);
  vlsir::spice::SimInput read_input;
  ASSERT_TRUE(read_input.ParseFromIstream(&input_file));
  EXPECT_EQ(read_input.top(), "second");
}

TEST_F(NetlisterTest, CreatesDirectoryIfNeeded) {
  // Use a nested directory that doesn't exist yet
  auto nested_dir = test_dir_ / "nested" / "path" / "test";
  std::filesystem::create_directories(nested_dir);

  vlsir::spice::SimInput sim_input;
  sim_input.set_top("nested_test");

  Flavour flavour = Flavour::XYCE;
  auto result = netlister_->WriteSim(sim_input, flavour, nested_dir);

  // Verify the file was created in the nested directory
  std::filesystem::path expected_file = nested_dir / "sim_input.pb";
  EXPECT_TRUE(std::filesystem::exists(expected_file));
}

}  // namespace
}  // namespace spiceserver
