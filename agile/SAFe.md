# Scaled Agile Framework (SAFe)

SAFe is a framework for scaling agile principles and practices across large enterprises. While agile works well for smaller teams, SAFe extends these principles to multiple teams, departments or even the entire organization. SAFe is a way to apply agile to enterprise scale, aligning teams to common goals, coordinating releases and enabling continuous delivery of value.

## Core Components - Levels

SAFe is built around 4 levels, depending on the size and needs of the organization. These levels aid organizations in the efficient adoption of agile to deliver on the value of corporate goals.

* Team
* Program (ART)
* Large Solution
* Portfolio

### Team Level

The Team level in SAFe operates similarly to Agile frameworks like **Scrum**, **Kanban**, or **XP**. Agile teams work in short, iterative cycles (usually 2 weeks), using practices such as **Sprint Planning**, **Daily Standups**, **Reviews**, and **Retrospectives**. These teams align their work with the larger **Program Increment (PI)** goals defined at the Program level.

Key characteristic of the Team level include:

* The goal is to deliver *customer value continuously*.
* Agile teams participate in common events like **Iteration Planning**, **Iteration Reviews**, and **System Demos**.
* **Extreme Programming (XP)** practices like pair programming and refactoring may be used to ensure technical excellence.
* **Continuous Integration** is required for enabling fast feedback and scalability.
* **Test-Driven Development (TDD)** is encouraged to ensure quality.
* A **Team Backlog** is maintained, consisting of User Stories derived from higher-level features.
* Teams conduct Iteration Planning and Iteration Retrospectives as part of the inspect-and-adapt loop.

### Program (ART) Level

The Program level is centered on the **Agile Release Train (ART)**—a team of Agile teams (usually 5–12) that work together to deliver larger value increments. ARTs align teams to a shared mission, operating in a synchronized cadence known as a **Program Increment (PI)**, which typically lasts 8–12 weeks.

Key characteristic of the Program level include:

* The **Program Backlog** holds prioritized Features (larger than stories), managed by Product Management.
* The ART follows a fixed cadence and aligns with business goals through regular **PI Planning** sessions.
* **WSJF (Weighted Shortest Job First)** is used to prioritize features based on economic value.
* Teams break down Features into **User Stories** for implementation during iterations.
* **System Demos** showcase integrated work from all teams at the end of every iteration.
* Cross-team collaboration is supported by roles like the **Release Train Engineer (RTE)** and **System Architect/Engineer**.
* ARTs deliver value incrementally and manage dependencies across teams.

### Large Solution Level

The large solution level is designed for developing complex systems that require multiple ARTs and possibly suppliers to deliver a single, integrated solution which is too big for just one ART. An example of this is areaspace systems, autonomous vehicles, telecom infrastructure - any solution where multiple teams and ARTs must synchronize at scale, with strong architectural and system integration needs.

Key characteristics of the Large Solution level include:

* Coordinates multiple ARTs and suppliers to build large, integrated solutions.
* **Solution Planning** and **Solution Demo** events enable synchronized progress across ARTs.
* Introduces **Capabilities** (larger than *features* but smaller than *epics*) as work items in the **Solution Backlog**.
* Uses **Solution Intent** to define what the solution should do (functional and non-functional requirements).
* There is a heavy importance placed on compliance, architecture and system integration.
* Encouragement of **Continuous Exploration and Integration** across ARTs.
* Maintains **Solution Kanban** to visualize and manage flow of large-scale work.

### Portfolio Level

The Portfolio level aligns the enterprise's strategy with execution by organizing work around **Value Streams**. This level is where strategic themes, investment funding, and portfolio vision are defined. It introduces **Lean Portfolio Management (LPM)** to ensure that initiatives deliver maximum business value and align with company goals.

Key characteristics of the Portfolio level include:

* Defines the organization’s *strategic objectives, value streams, and investment guardrails*.
* **Epics** (large initiatives) are managed in the **Portfolio Backlog** and follow a Lean Business Case process.
* Emphasis on *lean budgeting and decentralized decision-making* for faster delivery.
* High-level roles such as **Enterprise Architect**, **Business Owners**, and **Lean Portfolio Management** guide execution.
* Uses **Lean Governance** to ensure that delivery stays within budget and aligns with compliance and risk management goals.
* Measures outcomes using **Lean Metrics** such as *flow, outcome KPIs, and OKRs*.

## Key Elements in SAFe

### Agile Release Train (ART)

This is a team of agile teams (about 50-125 people) that plan, commit and deliver together on a fixed schedule known as a *Program Increment (PI)*. ART aligns people to a shared mission and cadence.

### Program Increment (PI)

A timebox (typically 8-12 weeks) where teams plan, build and deliver together. This starts with *PI Planning* which is a big-room event where teams align.

### Roles in SAFe

**Release Train Engineer (RTE)**: Scrum Master for the ART, *facilitating* PI planning and syncs.

**Product Manager**: Owns the program backlog and *prioritizes* features for the ART.

**System Architect/Engineer**: Provides technical guidance for the ART.

**Business Owners**: Stakeholders who evaluate and approve PI objectives.

**Scrum Master**: A facilitator and leader in a Scrum team to help the team achieve its goals.

**Product Owner** Owns the product backlog and *prioritizes* features for the product.

**Agile Team Member**: A member of a cross-functional group with the skills to define, build, test and deliver the product features.