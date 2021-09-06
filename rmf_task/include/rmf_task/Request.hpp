/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef RMF_TASK__REQUEST_HPP
#define RMF_TASK__REQUEST_HPP

#include <memory>

#include <rmf_task/Constraints.hpp>
#include <rmf_task/Parameters.hpp>
#include <rmf_task/State.hpp>
#include <rmf_task/Estimate.hpp>
#include <rmf_task/Priority.hpp>

#include <rmf_traffic/Time.hpp>

#include <rmf_utils/impl_ptr.hpp>

namespace rmf_task {

//==============================================================================
class Request
{
public:

  class Tag
  {
  public:

    /// Constructor
    ///
    /// \param[in] id_
    ///   The identify of the request
    ///
    /// \param[in] earliest_start_time_
    ///   The earliest time that the request may begin
    ///
    /// \param[in] priority_
    ///   The priority of the request
    ///
    /// \param[in] automatic_
    ///   Whether this request was automatically generated
    Tag(
      std::string id_,
      rmf_traffic::Time earliest_start_time_,
      ConstPriorityPtr priority_,
      bool automatic_ = false);

    /// The unique id for this request
    const std::string& id() const;

    /// Get the earliest time that this request may begin
    rmf_traffic::Time earliest_start_time() const;

    /// Get the priority of this request
    ConstPriorityPtr priority() const;

    // Returns true if this request was automatically generated
    bool automatic() const;

    class Implementation;
  private:
    rmf_utils::impl_ptr<Implementation> _pimpl;
  };

  using ConstTagPtr = std::shared_ptr<const Tag>;

  /// An abstract interface for computing the estimate and invariant durations
  /// of this request
  class Model
  {
  public:

    /// Estimate the state of the robot when the request is finished along with
    /// the time the robot has to wait before commencing the request
    virtual std::optional<Estimate> estimate_finish(
      const State& initial_state,
      const Constraints& task_planning_constraints,
      EstimateCache& estimate_cache) const = 0;

    /// Estimate the invariant component of the request's duration
    virtual rmf_traffic::Duration invariant_duration() const = 0;

    virtual ~Model() = default;
  };

  /// An abstract interface to define the specifics of this request. This
  /// implemented description will differentiate this request from others.
  class Description
  {
  public:

    /// Generate a Model for this request based on the unique traits of this
    /// description
    ///
    /// \param[in] earliest_start_time
    ///   The earliest time this request should begin execution. This is usually
    ///   the requested start time for the request.
    ///
    /// \param[in] parameters
    ///   The parameters that describe this AGV
    virtual std::shared_ptr<Model> make_model(
      rmf_traffic::Time earliest_start_time,
      const Parameters& parameters) const = 0;

    // Virtual destructor
    virtual ~Description() = default;
  };

  using DescriptionPtr = std::shared_ptr<Description>;
  using ConstDescriptionPtr = std::shared_ptr<const Description>;

  /// Constructor
  ///
  /// \param[in] earliest_start_time
  ///   The desired start time for this request
  ///
  /// \param[in] priority
  ///   The priority for this request. This is provided by the Priority Scheme.
  ///   For requests that do not have any priority this is a nullptr.
  ///
  /// \param[in] description
  ///   The description for this request
  ///
  /// \param[in] automatic
  ///   True if this request is auto-generated
  //
  // TODO(MXG): Deprecate this constructor?
  Request(
    const std::string& id,
    rmf_traffic::Time earliest_start_time,
    ConstPriorityPtr priority,
    ConstDescriptionPtr description,
    bool automatic = false);

  /// Constructor
  ///
  /// \param[in] tag
  ///   Tag of the request
  ///
  /// \param[in] description
  ///   Description for this request
  Request(ConstTagPtr tag, ConstDescriptionPtr description);

  /// Get the tag of this request
  const ConstTagPtr& tag() const;

  /// Get the description of this request
  const ConstDescriptionPtr& description() const;

  class Implementation;
private:
  rmf_utils::impl_ptr<Implementation> _pimpl;
};

using RequestPtr = std::shared_ptr<Request>;
using ConstRequestPtr = std::shared_ptr<const Request>;
using DescriptionPtr = Request::DescriptionPtr;
using ConstDescriptionPtr = std::shared_ptr<const Request::Description>;

} // namespace rmf_task

#endif // RMF_TASK__REQUEST_HPP
